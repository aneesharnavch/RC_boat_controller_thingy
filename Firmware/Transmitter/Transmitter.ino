#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>
#include <Adafruit_NeoPixel.h>
#include "radio_protocol.h"

// defining pinouts 

static const int PIN_NRF_CE = 4;
static const int PIN_NRF_CSN = 5;
static const int PIN_NRF_MISO = 6;
static const int PIN_NRF_SCK = 12;
static const int PIN_NRF_MOSI = 13;
static const int PIN_NRF_IRQ = 14; 
// pin 14 is not actively used by the polling driver and is left as an input pin 

// Joystick 1
static const int PIN_VRX1 = 1;
static const int PIN_VRY1 = 2;
static const int PIN_SW1 = 21;

// Joystick 2 
static const int PIN_VRX2 = 3;
static const int PIN_VRY2 = 36;
static const int PIN_SW2 = 11; 

// Push Button (pulled up to 3.3V)
static const int PIN_PB0 = 15;
static const int PIN_PB1 = 7;

//WS2812B status LED 
static const int PIN_LED = 48;
static const int NUM_LED = 3; 

// CONNECTING RADIO THINGY , MUST MATCH BOAT PLEASE CHANGE ACCORDINGLY , WILL NOT WORK IF YOUVE CHANGED THE SLIGHTEST THINGY 
static const uint8_t NRF_ADDRESS[5] = {'B','O','A','T','1'};
static const uint8_t NRF_RF_CHANNEL = 76;
#define NRF_PA_LEVEL RF24_PA_LOW

// Time steps
static const uint32_t TX_PERIOD_MS = 20; 
static const uint32_t ARM_HOLD_MS = 3500;

// Stick callibration 
static const int ADC_MAX = 4095;
static const uint16_t US_MIN = 1000;
static const uint16_t US_MID = 1500;
static const uint16_t US_MAX = 2000; 
static const int STEER_DEADBAND  = 60;     

// Reverse switches 
static const bool     INVERT_THROTTLE = false;   // bottom of travel must = 1000us
static const bool     INVERT_STEERING = false;

// Global
RF24 radio(PIN_NRF_CE, PIN_NRF_CSN);
Adafruit_NeoPixel leds(NUM_LED, PIN_LED, NEO_GRB + NEO_KHZ800);
RadioCommandPacket p;

// first boot config settings 
uint16_t gSeq = 0;
int gSteerCenter = ADC_MAX / 2;  
uint8_t gMode = 0;                 
bool gArmFlag = false;             
bool gKill = false;   

// startup and arming gate , addingh buffer time when pinned at idle on the boats ESC
bool gArmGate = true;
uint32_t gBootMs = 0;
bool gSeenIdle = false;
bool gLinkOk = false;   
uint32_t gLastTxMs = 0;

// Button debounce 
struct Button {
  int      pin;
  bool     lastStable;     // true = released (HIGH)
  bool     lastRead;
  uint32_t lastChangeMs;
};

Button btnPB0{PIN_PB0, true, true, 0};
Button btnPB1{PIN_PB1, true, true, 0};
Button btnSW1{PIN_SW1, true, true, 0};

// Returns true on a press edge (released -> pressed)
bool buttonPressed(Button &b) {
  bool reading = digitalRead(b.pin);   // HIGH = released, LOW = pressed
  uint32_t now = millis();
  if (reading != b.lastRead) {
    b.lastRead = reading;
    b.lastChangeMs = now;
  }
  bool edge = false;
  if ((now - b.lastChangeMs) > 25) {   // 25ms debounce
    if (reading != b.lastStable) {
      b.lastStable = reading;
      if (reading == LOW) edge = true; // just pressed
    }
  }
  return edge;
}

// Joystick reading 
int readAdcAvg(int pin) {
  long acc = 0;
  for (int i = 0; i < 4; i++) acc += analogRead(pin);
  return (int)(acc / 4);
}

uint16_t readThrottleUs() {
  int raw = readAdcAvg(PIN_VRY1);
  if (INVERT_THROTTLE) raw = ADC_MAX - raw;
  long us = US_MIN + (long)raw * (US_MAX - US_MIN) / ADC_MAX;
  if (us < US_MIN) us = US_MIN;
  if (us > US_MAX) us = US_MAX;
  return (uint16_t)us;
}

uint16_t readSteeringUs() {
  int raw = readAdcAvg(PIN_VRX2);
  if (INVERT_STEERING) raw = ADC_MAX - raw;
  int delta = raw - gSteerCenter;
  if (delta > -STEER_DEADBAND && delta < STEER_DEADBAND) return US_MID;
  long us;
  if (delta >= 0) {
    long span = (ADC_MAX - gSteerCenter);
    if (span < 1) span = 1;
    us = US_MID + (long)(delta - STEER_DEADBAND) * (US_MAX - US_MID) / span;
  } else {
    long span = gSteerCenter;
    if (span < 1) span = 1;
    us = US_MID + (long)(delta + STEER_DEADBAND) * (US_MID - US_MIN) / span;
  }
  if (us < US_MIN) us = US_MIN;
  if (us > US_MAX) us = US_MAX;
  return (uint16_t)us;
}

// Status LEDs 
uint32_t modeColor(uint8_t m){
  switch (m){
    case 0: return leds.Color(0, 40, 0);    // MANUAL  - green
    case 1: return leds.Color(0, 30, 40);   // ASSISTED   - cyan
    case 2: return leds.Color(40, 25, 0);   // HEADING_HOLD - amber
    case 3: return leds.Color(30, 0, 40);   // COURSE_HOLD  - purple
  }
  return 0;
}

void updateLeds(uint16_t throttleUs) {
  if (gKill) leds.setPixelColor(0, leds.Color(60, 0, 0));      // red  = killed
  else if (gArmGate) leds.setPixelColor(0, leds.Color(50, 30, 0));    // amber= arming
  else if (gLinkOk) leds.setPixelColor(0, leds.Color(0, 50, 0));     // green= linked
  else leds.setPixelColor(0, leds.Color(50, 0, 30));    // pink = no ack
// led1 state
  leds.setPixelColor(1, modeColor(gMode));
// led2 throttle level 
  uint8_t lvl = (uint8_t)((long)(throttleUs - US_MIN) * 60 / (US_MAX - US_MIN));
  leds.setPixelColor(2, leds.Color(lvl, lvl, lvl));
  leds.show();
}

// setup 
void setup(){
  Serial.begin(115200);

  //button decleration 
  pinMode(PIN_PB0, INPUT_PULLUP);
  pinMode(PIN_PB1, INPUT_PULLUP);
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW2, INPUT_PULLUP);
  pinMode(PIN_NRF_IRQ, INPUT);

  // ADC 
  analogReadResolution(12);
  analogSetAttenuation(ADC_11db);

  //LEDS
  leds.begin();
  leds.clear();
  leds.show();

  // Capture steering center (assume stick released/centered at boot).
  delay(50);
  long acc = 0;
  for (int i = 0; i < 32; i++) { acc += analogRead(PIN_VRX2); delay(1); }
  gSteerCenter = (int)(acc / 32);

  // SPI on the controllers custom pins
  SPI.begin(PIN_NRF_SCK, PIN_NRF_MISO, PIN_NRF_MOSI, PIN_NRF_CSN);
  if (!radio.begin(&SPI)) {
    Serial.println(F("[nRF24] begin() FAILED - check wiring/power"));
  }
  radio.setDataRate(RF24_250KBPS);
  radio.setChannel(NRF_RF_CHANNEL);
  radio.setPayloadSize(sizeof(RadioCommandPacket));
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPALevel(NRF_PA_LEVEL);
  // Dynamic payloads intentionally left OFF (default).
  radio.openWritingPipe(NRF_ADDRESS);
  radio.stopListening();

  gBootMs   = millis();
  gLastTxMs = gBootMs;

  Serial.println(F("[TX] Transmitter ready. Holding idle throttle for ESC arming..."));
  Serial.println(F("[TX] Pull throttle stick to the BOTTOM to complete arming."));

}

//  Main loop  (fixed 50 Hz cadence)
void loop(){
  uint32_t now = millis();
  if (buttonPressed(btnPB0)) {        // cycle mode
    gMode = (gMode + 1) & 0x03;
    Serial.printf("[TX] mode -> %u\n", gMode);
  }
  if (buttonPressed(btnPB1)) {                 // toggle arm flag
    gArmFlag = !gArmFlag;
    Serial.printf("[TX] arm flag -> %d\n", gArmFlag);
  }
  if (buttonPressed(btnSW1)) {                 // joystick-1 click = kill toggle
    gKill = !gKill;                            // latches RADIO_FLAG_KILL + forces idle throttle
    Serial.printf("[TX] kill -> %d\n", gKill);
  }

  if (now - gLastTxMs < TX_PERIOD_MS) return;  // hold 50 Hz
  gLastTxMs += TX_PERIOD_MS;
  if (now - gLastTxMs > TX_PERIOD_MS) gLastTxMs = now;  

  // read sticks 
  uint16_t rawThrottle = readThrottleUs();
  uint16_t steeringUs  = readSteeringUs();
  
  if (gArmGate) {
    if (rawThrottle <= US_MIN + 40) gSeenIdle = true;
    if (gSeenIdle && (now - gBootMs >= ARM_HOLD_MS)) {
      gArmGate = false;
      Serial.println(F("[TX] arming complete - throttle live"));
    }
  }

  uint16_t throttleUs = gArmGate ? US_MIN : rawThrottle;
  if (gKill) throttleUs = US_MIN;

  // make packet 
  p.magic = RADIO_MAGIC;
  p.seq = gSeq++;
  p.throttleUs = throttleUs;
  p.steeringUs = steeringUs;
  p.headingDeg = 0;        // populated only if a heading source is added
  p.speedCmps = 0;        // 0 = speed-hold disabled
  p.mode = gMode;
  p.flags = (gArmFlag ? RADIO_FLAG_ARM : 0) | (gKill    ? RADIO_FLAG_KILL : 0);
  p.checksum   = radioChecksum(p);   
  gLinkOk = radio.write(&p, sizeof(p));   // true if ack received

  updateLeds(throttleUs);
}
