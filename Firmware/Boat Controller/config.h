#pragma once

#include <Arduino.h>

static constexpr uint32_t SERIAL_BAUD = 115200;
static constexpr uint32_t BOOT_DELAY_MS = 400;

static constexpr uint8_t PIN_I2C_SDA = 4;
static constexpr uint8_t PIN_I2C_SCL = 5;

static constexpr uint8_t PIN_SERVO_SIGNAL = 18;
static constexpr uint8_t PIN_ESC_SIGNAL = 19;

static constexpr uint8_t PIN_GPS_RX = 9;
static constexpr uint8_t PIN_GPS_TX = 8;
static constexpr uint32_t GPS_BAUD = 9600;

static constexpr uint8_t ICM20948_ADDR = 0x69;
static constexpr uint8_t BMP388_ADDR = 0x77;

// --------------------------
// nRF24L01 command radio
// --------------------------
// The nRF24L01 is an SPI device. CE and CSN can be ANY free GPIO; the
// SCK / MOSI / MISO pins MUST be valid pins for the SPI bus selected by
// NRF_USE_SPI1 below (RP2040 has fixed pin groups per SPI peripheral).
//
// >>> SET ALL FIVE PINS TO MATCH YOUR WIRING. The values below are only
// >>> placeholders that avoid clashing with the I2C/servo/GPS pins. <<<
static constexpr bool USE_NRF24 = true;
static constexpr bool NRF_USE_SPI1 = false;  // false = SPI0, true = SPI1

static constexpr uint8_t PIN_NRF_CE = 1;     // TODO: your wiring (any GPIO)
static constexpr uint8_t PIN_NRF_CSN = 7;    // TODO: your wiring (any GPIO)
static constexpr uint8_t PIN_NRF_SCK = 2;    // TODO: must be a SCK pin of the chosen SPI bus
static constexpr uint8_t PIN_NRF_MOSI = 3;   // TODO: must be a TX  pin of the chosen SPI bus
static constexpr uint8_t PIN_NRF_MISO = 0;   // TODO: must be a RX  pin of the chosen SPI bus

// RF link settings (must match the transmitter exactly).
static constexpr uint8_t NRF_RF_CHANNEL = 76;  // 0..125 (avoid common WiFi channels)
static constexpr uint8_t NRF_ADDRESS[5] = {'B', 'O', 'A', 'T', '1'};

static constexpr float IMU_HZ = 200.0f;
static constexpr float BARO_HZ = 25.0f;
static constexpr float GPS_HZ = 20.0f;
static constexpr float CONTROL_HZ = 100.0f;
static constexpr float TELEMETRY_HZ = 10.0f;
static constexpr float COMMAND_HZ = 50.0f;

static constexpr uint32_t IMU_PERIOD_US = static_cast<uint32_t>(1000000.0f / IMU_HZ);
static constexpr uint32_t BARO_PERIOD_US = static_cast<uint32_t>(1000000.0f / BARO_HZ);
static constexpr uint32_t GPS_PERIOD_US = static_cast<uint32_t>(1000000.0f / GPS_HZ);
static constexpr uint32_t CONTROL_PERIOD_US = static_cast<uint32_t>(1000000.0f / CONTROL_HZ);
static constexpr uint32_t TELEMETRY_PERIOD_US = static_cast<uint32_t>(1000000.0f / TELEMETRY_HZ);
static constexpr uint32_t CMD_PERIOD_US = static_cast<uint32_t>(1000000.0f / COMMAND_HZ);
static constexpr uint32_t HEARTBEAT_PERIOD_US = 250000;

// --------------------------
// Feature flags
// --------------------------
static constexpr bool USE_BMP388 = true;
static constexpr bool USE_GPS = true;
static constexpr bool USE_MAG_YAW = false;
static constexpr bool ENABLE_HEADING_HOLD = true;
static constexpr bool ENABLE_SPEED_HOLD = true;
static constexpr bool ENABLE_RATE_DAMPING = true;

// --------------------------
// Servo and ESC
// --------------------------
static constexpr int SERVO_US_MIN = 1100;
static constexpr int SERVO_US_CENTER = 1500;
static constexpr int SERVO_US_MAX = 1900;
static constexpr int SERVO_MAX_DEFLECT_US = 340;
static constexpr int SERVO_MAX_SLEW_US_PER_TICK = 10;

static constexpr int ESC_US_MIN = 1000;
static constexpr int ESC_US_IDLE = 1100;
static constexpr int ESC_US_MAX = 2000;
static constexpr int ESC_ARM_US = 1000;
static constexpr uint16_t ESC_ARM_TIME_MS = 2500;
static constexpr int ESC_MAX_SLEW_US_PER_TICK = 8;

// --------------------------
// Safety thresholds
// --------------------------
static constexpr float MAX_ABS_ROLL_DEG_FOR_FULL_THR = 35.0f;
static constexpr float MAX_ABS_PITCH_DEG_FOR_FULL_THR = 20.0f;
static constexpr float FAILSAFE_THROTTLE_FACTOR = 0.55f;

static constexpr uint32_t SENSOR_TIMEOUT_IMU_US = 120000;
static constexpr uint32_t SENSOR_TIMEOUT_BARO_US = 300000;
static constexpr uint32_t SENSOR_TIMEOUT_GPS_US = 1500000;
static constexpr uint32_t COMMAND_TIMEOUT_US = 500000;

// Estimator tuning

static constexpr float ACCEL_LPF_ALPHA = 0.24f;
static constexpr float GYRO_LPF_ALPHA = 0.30f;
static constexpr float BARO_LPF_ALPHA = 0.15f;
static constexpr float COMPLEMENTARY_ALPHA = 0.98f;

// PID tuning

static constexpr float PID_HEADING_KP = 2.00f;
static constexpr float PID_HEADING_KI = 0.06f;
static constexpr float PID_HEADING_KD = 0.03f;
static constexpr float PID_HEADING_I_LIM = 35.0f;
static constexpr float PID_HEADING_OUT_LIM = 45.0f;  // deg/s yaw target

static constexpr float PID_YAWRATE_KP = 2.60f;
static constexpr float PID_YAWRATE_KI = 0.25f;
static constexpr float PID_YAWRATE_KD = 0.02f;
static constexpr float PID_YAWRATE_I_LIM = 30.0f;
static constexpr float PID_YAWRATE_OUT_LIM = 1.0f;   // normalized steer

static constexpr float PID_SPEED_KP = 0.50f;
static constexpr float PID_SPEED_KI = 0.10f;
static constexpr float PID_SPEED_KD = 0.00f;
static constexpr float PID_SPEED_I_LIM = 0.40f;
static constexpr float PID_SPEED_OUT_LIM = 0.50f;    // throttle trim

// --------------------------
// Serial command protocol
// --------------------------
static constexpr size_t CMD_BUFFER_LEN = 96;
static constexpr char CMD_DELIM = '\n';