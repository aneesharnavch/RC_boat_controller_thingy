# Controller / Transmitter Handover

**Purpose:** everything needed to build the **transmitter** (hand controller) that
drives the RC Boat Thingy over an nRF24L01 link. The boat-side (receiver)
firmware already exists and is described below. Your job, if you're the agent
reading this, is to build a transmitter that produces packets the boat accepts.

> The single most important rule: **the transmitter and the boat must agree
> byte-for-byte on the radio settings and packet layout.** The boat side is the
> source of truth. Match it exactly.

---

## 1. System overview (boat side)

The boat runs an RP2040 (Earle Philhower arduino-pico core) with this firmware
layout:

| File | Role |
|------|------|
| `Boat_Controller.ino` | Cooperative scheduler; runs sensor/control/telemetry/command tasks on fixed periods. |
| `config.h` | All tunables and pin assignments, including the nRF24 block. |
| `types.h` | Shared structs/enums (`CommandInput`, `Mode`, etc.). |
| `sensors.*` | ICM-20948 IMU, BMP388 baro, GPS. |
| `estimator.*` | Complementary-filter attitude + speed/alt fusion. |
| `control.*` | PID controllers (heading / yaw-rate / speed), output mixing. |
| `actuator.*` | Servo (rudder) + ESC (motor) drive, arming, slew limiting. |
| `failsafe.*` | Timeout/validity monitoring → failsafe state. |
| `commands.*` | **Serial** command parser (text protocol, for bench use). |
| `radio.*` | **nRF24L01** command receiver (what your transmitter talks to). |
| `radio_protocol.h` | **Shared wire format. Copy this file into your transmitter project.** |
| `boat_profile.h/.c` | Optional generated tuning profile (input shaping lives here). |

Commands from **either** the serial parser **or** the radio land in the same
`CommandInput gCmd` struct. The control loop is agnostic to the source. Your
transmitter only needs to care about the radio path.

The command task runs at **50 Hz** (`COMMAND_HZ` in `config.h`) and polls the
radio every cycle.

---

## 2. The radio link contract (MUST MATCH)

These are set on the boat in `radio.cpp` (`initRadio()`) and `config.h`. Your
transmitter must configure the RF24 identically, except where noted.

| Setting | Boat value | Notes |
|---------|-----------|-------|
| Library | RF24 by TMRh20 | Use the same library on both ends. |
| Data rate | `RF24_250KBPS` | Must match. |
| RF channel | `NRF_RF_CHANNEL = 76` | `config.h`; must match. 0–125. |
| Address (5 bytes) | `NRF_ADDRESS = {'B','O','A','T','1'}` | = `0x42 0x4F 0x41 0x54 0x31`. Must match. |
| Address width | 5 (RF24 default) | Don't change. |
| CRC | RF24 default (16-bit, enabled) | Leave default on both ends. |
| Auto-ack | `setAutoAck(true)` | Must match. |
| Payload | **static**, `setPayloadSize(15)` | Boat uses fixed 15-byte payload. **Do NOT enable dynamic payloads.** Send exactly 15 bytes. |
| PA level | `RF24_PA_LOW` | Does **not** need to match; tune per range/power. |
| Retries | `setRetries(5, 15)` | Matters for the *transmitter* (it's the sender). Use the same as a starting point. |
| Role | Boat = PRX (`startListening()`) | Transmitter = PTX. |

**Pipe / address direction:** the boat does `openReadingPipe(1, NRF_ADDRESS)`.
The transmitter must `openWritingPipe(NRF_ADDRESS)` with the **identical 5-byte
array**. With auto-ack, RF24 handles the ack pipe automatically — no extra setup.

The boat's nRF **pins** are user-set in `config.h` (`PIN_NRF_CE`, `PIN_NRF_CSN`,
`PIN_NRF_SCK`, `PIN_NRF_MOSI`, `PIN_NRF_MISO`, and `NRF_USE_SPI1`). Your
transmitter's pins are independent — wire it however you like and configure its
own RF24 accordingly. **Channel and address are the only things that must match,
not pins.**

---

## 3. Packet format

Defined in `radio_protocol.h`. **Copy that file into your transmitter project
verbatim** and build the packet with the same struct — that eliminates any
byte-order or padding mistakes. The struct is `__attribute__((packed))`,
**15 bytes**, sent raw, **little-endian** (both ends are little-endian MCUs).

```c
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
```

### Field semantics

| Field | Range / units | Meaning |
|-------|---------------|---------|
| `magic` | `0xB0A7` | Constant sanity tag. Packets with any other value are dropped. |
| `seq` | 0..65535, wraps | Increment once per transmitted frame. Used for link-quality/telemetry; the boat does not require monotonicity. |
| `throttleUs` | 1000–2000 µs | RC-style pulse. **1000 = idle/stop, 2000 = full.** See §4 for shaping. |
| `steeringUs` | 1000–2000 µs | RC-style pulse. **1500 = center**, 1000 = full left, 2000 = full right (reverse handled boat-side). |
| `headingDeg` | 0–359 | Target compass heading; only used in `MODE_HEADING_HOLD`. |
| `speedCmps` | cm/s | Target speed; only used when > 0 **and** GPS has a fix. `0` disables speed hold. (e.g. 2.5 m/s → `250`). |
| `mode` | 0–3 | See enum below. Values outside 0–3 are ignored (mode unchanged). |
| `flags` | bitmask | `0x01` arm, `0x02` kill. |
| `checksum` | XOR | `radioChecksum()` over bytes 0..13. Boat drops packets that fail. |

### Mode enum (from `types.h`)

| Value | Mode | Behavior |
|-------|------|----------|
| 0 | `MODE_MANUAL` | Raw throttle + steering passthrough. |
| 1 | `MODE_ASSISTED` | Steering becomes a yaw-rate command (rate damping). |
| 2 | `MODE_HEADING_HOLD` | Holds `headingDeg` via PID. |
| 3 | `MODE_COURSE_HOLD` | Holds GPS course. |

### Checksum

```c
static inline uint8_t radioChecksum(const RadioCommandPacket &p) {
  const uint8_t *bytes = reinterpret_cast<const uint8_t *>(&p);
  uint8_t c = 0;
  for (size_t i = 0; i < sizeof(RadioCommandPacket) - 1; i++) c ^= bytes[i];
  return c;
}
```

Fill `checksum` **last**, after every other field is set.

---

## 4. How the boat interprets throttle/steering

The boat converts the µs pulses to normalized commands. **Send clean, standard
RC pulses (1000/1500/2000) and let the boat do the shaping** — do not pre-apply
expo/deadband on the transmitter, or it will be applied twice.

- **If `boat_profile.h` was generated** (the usual case): pulses run through
  `bp_shape_throttle()` / `bp_shape_steering()`, which apply the profile's
  **deadband, expo, reverse flags, and throttle scaling**. Centers/endpoints come
  from `BP_RC_MIN_US`/`BP_RC_MID_US`/`BP_RC_MAX_US` (default 1000/1500/2000).
- **If no profile exists** (fallback in `radio.cpp`): throttle = `(us-1000)/1000`
  clamped 0..1; steering = `(us-1500)/500` clamped -1..1.

Either way, the transmitter's job is just: **map sticks to 1000–2000 µs, throttle
bottom = 1000, steering center = 1500.**

---

## 5. Arming, kill, and failsafe (transmitter must respect these)

### ESC arming is throttle-based (automatic)
On boot the boat is `ARM_DISARMED`. Arming happens in `actuator.cpp`:
- It moves to `ARM_ARMING` only once throttle is at/near idle (≤ ~0.01).
- It holds zero throttle for `ESC_ARM_TIME_MS` (**2500 ms**), then becomes
  `ARM_ARMED`.
- If throttle is raised while disarmed, throttle is forced to 0.

**Transmitter requirement:** at startup, send **idle throttle (1000 µs)** and
wait ~3 s before commanding throttle. The `RADIO_FLAG_ARM` bit is currently
*informational* — arming is gated by idle throttle, not by the flag. (Set the
flag anyway; future firmware may consume it.)

### Kill
Setting `RADIO_FLAG_KILL` makes the boat latch failsafe (motor + steering zeroed,
disarmed). The boat re-reads the flag every packet, so **clearing the kill bit in
a later packet releases the kill** (you then re-arm via idle throttle). Design
your controller's kill switch with that in mind (e.g. momentary vs latching).

### Command-timeout failsafe (critical)
The boat triggers `FAIL_CMD_TIMEOUT` if no valid command arrives within
`COMMAND_TIMEOUT_US` = **500 ms** (`config.h`). On timeout the motor and steering
are zeroed.

**Transmitter requirement:** send packets continuously at a steady rate even when
nothing changes. **Recommended: 50 Hz** (matches the boat's command task; one
packet every 20 ms). Anything slower than 2 Hz will trip the failsafe. 20–50 Hz
is the sweet spot.

---

## 6. Reference transmitter logic

Pseudocode for a minimal correct transmitter (real Arduino/RF24 on any
little-endian MCU; RP2040, AVR, ESP32 all fine):

```c
#include <SPI.h>
#include <RF24.h>
#include "radio_protocol.h"   // copied from the boat project

const uint8_t ADDRESS[5] = {'B','O','A','T','1'};   // MUST match boat
RF24 radio(TX_CE_PIN, TX_CSN_PIN);                   // your wiring

uint16_t seq = 0;

void setup() {
  // (configure SPI pins for your board if needed)
  radio.begin();
  radio.setDataRate(RF24_250KBPS);   // match boat
  radio.setChannel(76);              // match boat NRF_RF_CHANNEL
  radio.setPayloadSize(sizeof(RadioCommandPacket));  // 15, static
  radio.setAutoAck(true);
  radio.setRetries(5, 15);
  radio.setPALevel(RF24_PA_LOW);     // tune for range
  radio.openWritingPipe(ADDRESS);
  radio.stopListening();             // PTX role
}

void loop() {
  RadioCommandPacket p = {};
  p.magic      = RADIO_MAGIC;
  p.seq        = seq++;
  p.throttleUs = readThrottleStick_1000_2000();   // 1000 at rest!
  p.steeringUs = readSteeringStick_1000_2000();    // 1500 center
  p.headingDeg = currentTargetHeading();           // 0..359 (or 0)
  p.speedCmps  = currentTargetSpeedCmps();         // 0 to disable
  p.mode       = currentMode();                    // 0..3
  p.flags      = (armSwitch ? RADIO_FLAG_ARM : 0)
               | (killSwitch ? RADIO_FLAG_KILL : 0);
  p.checksum   = radioChecksum(p);                 // last

  radio.write(&p, sizeof(p));

  delay(20);   // ~50 Hz; keep it steady to avoid the 500 ms failsafe
}
```

### Startup sequence the transmitter should enforce
1. Power on with throttle stick at idle (1000 µs).
2. Begin transmitting at 50 Hz immediately (idle throttle).
3. Hold idle ≥ 3 s so the boat's ESC arms.
4. Then allow throttle commands.

---

## 7. Boat-side values you can change (and must keep in sync)

If you edit these on the boat, mirror them on the transmitter:

| `config.h` constant | Default | Sync? |
|--------------------|---------|-------|
| `NRF_RF_CHANNEL` | 76 | Yes — must match. |
| `NRF_ADDRESS` | `{'B','O','A','T','1'}` | Yes — must match. |
| `USE_NRF24` | `true` | If false, boat ignores the radio entirely. |
| `NRF_USE_SPI1` | `false` | Boat-local SPI bus choice; not shared. |
| `PIN_NRF_*` | placeholders | Boat-local wiring; not shared. **Set these to your boat's wiring.** |
| `COMMAND_TIMEOUT_US` | 500000 | If you raise/lower it, adjust min TX rate. |
| Data rate / PA / payload | in `radio.cpp` | Data rate + payload must match; PA need not. |

---

## 8. Gotchas

- **Don't enable dynamic payloads** on the transmitter — the boat uses a fixed
  15-byte payload. Mismatched payload handling = nothing received.
- **Checksum is filled last.** Compute it after all fields are set, or it'll be
  wrong and every packet will be silently dropped.
- **Throttle must idle at 1000 µs at startup**, or the ESC never arms.
- **Keep transmitting even when idle** — silence > 500 ms = failsafe.
- **Don't double-shape inputs.** Expo/deadband/reverse happen on the boat. Send
  linear 1000–2000 pulses.
- **Endianness/packing:** reuse `radio_protocol.h` and send the struct raw. Only
  hand-pack bytes if your TX MCU is big-endian (none of the common Arduino MCUs
  are) — in which case serialize each field little-endian explicitly.
- **Telemetry is one-way for now.** The boat prints telemetry over USB serial
  (see `telemetry.cpp`); it does **not** send telemetry back over the nRF link.
  If you want radio telemetry, that's a new feature on both ends (ack payloads or
  a second pipe) — not currently implemented.

---

## 9. File checklist for the transmitter project

- Copy **`radio_protocol.h`** from the boat firmware into the transmitter
  project. It is dependency-free (only `<stdint.h>`/`<stddef.h>`).
- Install the **RF24 (TMRh20)** library.
- Match **channel 76** and **address `{'B','O','A','T','1'}`**, `RF24_250KBPS`,
  static 15-byte payload, auto-ack on.
- Transmit `RadioCommandPacket` at ~50 Hz with a valid `magic` and `checksum`.

That's the entire contract. If the boat firmware's radio settings change, this
document and `radio_protocol.h` are the two things to re-check.
