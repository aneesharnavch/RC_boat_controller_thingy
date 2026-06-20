// radio_protocol.h
// Shared wire format between the RC boat (receiver / PRX) and this
// transmitter (PTX). Must stay byte-for-byte identical to the boat's copy at
// Boat_Controller/radio_protocol.h -- if one side changes, change both.

#ifndef RADIO_PROTOCOL_H
#define RADIO_PROTOCOL_H

#include <stdint.h>
#include <stddef.h>

static constexpr uint16_t RADIO_MAGIC = 0xB0A7;

enum RadioFlag : uint8_t {
  RADIO_FLAG_ARM  = 1 << 0,   // 0x01
  RADIO_FLAG_KILL = 1 << 1,   // 0x02
};

struct __attribute__((packed)) RadioCommandPacket {
  uint16_t magic;        // offset 0  : must equal RADIO_MAGIC (0xB0A7)
  uint16_t seq;          // offset 2  : increment every frame
  uint16_t throttleUs;   // offset 4  : 1000..2000
  uint16_t steeringUs;   // offset 6  : 1000..2000
  uint16_t headingDeg;   // offset 8  : 0..359
  uint16_t speedCmps;    // offset 10 : target speed in cm/s (0 = off)
  uint8_t  mode;         // offset 12 : Mode enum 0..3
  uint8_t  flags;        // offset 13 : RadioFlag bits
  uint8_t  checksum;     // offset 14 : XOR of bytes [0..13]
};

static_assert(sizeof(RadioCommandPacket) == 15, "packet must be 15 bytes");

// XOR over every byte except the trailing checksum byte. Fill checksum LAST.
static inline uint8_t radioChecksum(const RadioCommandPacket &p) {
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&p);
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(RadioCommandPacket) - 1; i++) c ^= bytes[i];
  return c;
}

#endif 
