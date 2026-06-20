#pragma once

// Shared nRF24L01 wire protocol for the Boat Controller command link.
//
// Include this file from BOTH the boat firmware and the transmitter sketch so
// the two ends agree byte-for-byte on the packet layout. It deliberately has
// no dependency on the rest of the firmware (only <stdint.h>/<stddef.h>) so it
// can be dropped straight into a separate transmitter project.

#include <stdint.h>
#include <stddef.h>

// Sanity tag at the head of every packet; lets the receiver reject stray RF
// payloads that happen to land on the pipe.
static constexpr uint16_t RADIO_MAGIC = 0xB0A7;

// Bit flags packed into RadioCommandPacket::flags.
enum RadioFlag : uint8_t {
  RADIO_FLAG_ARM = 1 << 0,   // request arm
  RADIO_FLAG_KILL = 1 << 1,  // request kill (latched failsafe)
};

// 15-byte fixed payload (well under the nRF24L01's 32-byte limit). Throttle and
// steering are carried as RC-style microsecond pulses so the receiver can run
// them through the same shaping/expo pipeline as a wired RC input.
struct __attribute__((packed)) RadioCommandPacket {
  uint16_t magic;        // == RADIO_MAGIC
  uint16_t seq;          // increments each frame (link-quality / dedup)
  uint16_t throttleUs;   // 1000..2000
  uint16_t steeringUs;   // 1000..2000
  uint16_t headingDeg;   // 0..359, target heading for hold modes
  uint16_t speedCmps;    // target speed, cm/s (0 = no speed hold)
  uint8_t mode;          // Mode enum value
  uint8_t flags;         // RadioFlag bits
  uint8_t checksum;      // XOR of all preceding bytes
};

// Compute the XOR checksum over every byte except the trailing checksum field.
// Both ends call this identically, so the transmitter fills it and the receiver
// verifies it.
static inline uint8_t radioChecksum(const RadioCommandPacket &p) {
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&p);
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(RadioCommandPacket) - 1; i++) {
    c ^= bytes[i];
  }
  return c;
}
