#pragma once

#include "types.h"
#include "radio_protocol.h"

// nRF24L01 command receiver. Wraps the RF24 library and feeds decoded packets
// into the shared CommandInput, exactly like the serial command parser does, so
// the rest of the firmware is agnostic to where commands came from.
//
// Requires the "RF24" library (TMRh20). Pins and RF settings live in config.h.

// Initialise SPI + the radio. Returns true if the chip responded and listening
// started. Safe to call when USE_NRF24 is false (returns false, does nothing).
bool initRadio();

// Drain any received packets and apply the newest valid one to cmd. Returns
// true if at least one valid packet was applied this call. Updates
// cmd.lastCommandUs on success so the existing command-timeout failsafe covers
// radio loss automatically.
bool pollRadio(BoatState &state, CommandInput &cmd, uint32_t nowUs);

// True while a valid packet has been received within COMMAND_TIMEOUT_US.
bool radioLinkOk(uint32_t nowUs);

// Sequence number of the last accepted packet (for telemetry / link quality).
uint16_t radioLastSeq();
