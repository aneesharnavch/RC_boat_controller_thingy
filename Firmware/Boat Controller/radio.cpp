#include "radio.h"
#include "config.h"

#include <SPI.h>
#include <RF24.h>

// Use the profile's input shaping (deadband + expo + throttle scaling) when a
// generated boat_profile is present; otherwise fall back to a plain linear map.
#if defined(__has_include)
#if __has_include("boat_profile.h")
#include "boat_profile.h"
#define HAVE_BOAT_PROFILE 1
#endif
#endif

static SPIClassRP2040 &sSpi = NRF_USE_SPI1 ? SPI1 : SPI;
static RF24 sRadio(PIN_NRF_CE, PIN_NRF_CSN);

static bool sRadioReady = false;
static uint32_t sLastRxUs = 0;
static uint16_t sLastSeq = 0;

static float clampf(float v, float lo, float hi) {
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

#ifndef HAVE_BOAT_PROFILE
// Minimal fallback mapping (standard 1000/1500/2000 us RC band) used when no
// boat_profile.h has been generated.
static float mapThrottle(uint16_t us) {
  float v = (static_cast<float>(us) - 1000.0f) / 1000.0f;
  return clampf(v, 0.0f, 1.0f);
}

static float mapSteering(uint16_t us) {
  float v = (static_cast<float>(us) - 1500.0f) / 500.0f;
  return clampf(v, -1.0f, 1.0f);
}
#endif

static void applyPacket(const RadioCommandPacket &p, BoatState &state,
                        CommandInput &cmd, uint32_t nowUs) {
#ifdef HAVE_BOAT_PROFILE
  cmd.throttle = bp_shape_throttle(p.throttleUs);
  cmd.steering = bp_shape_steering(p.steeringUs);
#else
  cmd.throttle = mapThrottle(p.throttleUs);
  cmd.steering = mapSteering(p.steeringUs);
#endif

  cmd.targetHeadingDeg = static_cast<float>(p.headingDeg);
  cmd.targetSpeedMps = static_cast<float>(p.speedCmps) * 0.01f;

  if (p.mode <= MODE_COURSE_HOLD) {
    cmd.requestedMode = static_cast<Mode>(p.mode);
  }

  cmd.armRequested = (p.flags & RADIO_FLAG_ARM) != 0;
  cmd.killRequested = (p.flags & RADIO_FLAG_KILL) != 0;
  if (cmd.killRequested) {
    state.armState = ARM_DISARMED;
  }

  cmd.lastCommandUs = nowUs;
}

bool initRadio() {
  sRadioReady = false;
  if (!USE_NRF24) {
    return false;
  }

  sSpi.setSCK(PIN_NRF_SCK);
  sSpi.setTX(PIN_NRF_MOSI);
  sSpi.setRX(PIN_NRF_MISO);
  sSpi.begin();

  if (!sRadio.begin(&sSpi)) {
    return false;
  }

  sRadio.setPALevel(RF24_PA_LOW);
  sRadio.setDataRate(RF24_250KBPS);
  sRadio.setChannel(NRF_RF_CHANNEL);
  sRadio.setPayloadSize(sizeof(RadioCommandPacket));
  sRadio.setAutoAck(true);
  sRadio.setRetries(5, 15);
  sRadio.openReadingPipe(1, NRF_ADDRESS);
  sRadio.startListening();

  sRadioReady = sRadio.isChipConnected();
  return sRadioReady;
}

bool pollRadio(BoatState &state, CommandInput &cmd, uint32_t nowUs) {
  if (!sRadioReady) {
    return false;
  }

  bool applied = false;
  RadioCommandPacket pkt;

  // Drain the FIFO; the newest valid packet wins (older ones are stale input).
  while (sRadio.available()) {
    sRadio.read(&pkt, sizeof(pkt));

    if (pkt.magic != RADIO_MAGIC) {
      continue;
    }
    if (radioChecksum(pkt) != pkt.checksum) {
      continue;
    }

    applyPacket(pkt, state, cmd, nowUs);
    sLastRxUs = nowUs;
    sLastSeq = pkt.seq;
    applied = true;
  }

  return applied;
}

bool radioLinkOk(uint32_t nowUs) {
  return sRadioReady && (nowUs - sLastRxUs) < COMMAND_TIMEOUT_US;
}

uint16_t radioLastSeq() {
  return sLastSeq;
}
