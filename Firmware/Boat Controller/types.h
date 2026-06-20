#pragma once

#include <Arduino.h>

enum SystemStatus : uint8_t {
  SYS_OK = 0,
  SYS_DEGRADED = 1,
  SYS_FAILSAFE = 2
};

enum Mode : uint8_t {
  MODE_MANUAL = 0,
  MODE_ASSISTED = 1,
  MODE_HEADING_HOLD = 2,
  MODE_COURSE_HOLD = 3
};

enum ArmState : uint8_t {
  ARM_DISARMED = 0,
  ARM_ARMING = 1,
  ARM_ARMED = 2
};

enum FailsafeReason : uint16_t {
  FAIL_NONE = 0,
  FAIL_IMU_TIMEOUT = 1 << 0,
  FAIL_IMU_INVALID = 1 << 1,
  FAIL_CMD_TIMEOUT = 1 << 2,
  FAIL_TILT_EXCESS = 1 << 3,
  FAIL_MANUAL_KILL = 1 << 4
};

struct ImuData {
  bool valid = false;
  float ax = 0.0f;
  float ay = 0.0f;
  float az = 0.0f;
  float gx = 0.0f;
  float gy = 0.0f;
  float gz = 0.0f;
  float mx = 0.0f;
  float my = 0.0f;
  float mz = 0.0f;
  uint32_t lastUpdateUs = 0;
};

struct BaroData {
  bool valid = false;
  float pressurePa = 0.0f;
  float temperatureC = 0.0f;
  float altitudeM = 0.0f;
  float pressureTrendPaPerSec = 0.0f;
  uint32_t lastUpdateUs = 0;
};

struct GpsData {
  bool valid = false;
  bool hasFix = false;
  uint8_t sats = 0;
  double lat = 0.0;
  double lon = 0.0;
  float speedMps = 0.0f;
  float courseDeg = 0.0f;
  float hdop = 99.9f;
  uint32_t ageMs = 0;
  uint32_t lastUpdateUs = 0;
};

struct SensorFrame {
  ImuData imu;
  BaroData baro;
  GpsData gps;
};

struct EstimatorState {
  float rollDeg = 0.0f;
  float pitchDeg = 0.0f;
  float yawDeg = 0.0f;
  float yawRateDps = 0.0f;
  float fusedSpeedMps = 0.0f;
  float filteredAltM = 0.0f;
};

struct CommandInput {
  float throttle = 0.0f;          // 0..1
  float steering = 0.0f;          // -1..1
  float targetHeadingDeg = 0.0f;
  float targetSpeedMps = 0.0f;
  bool armRequested = false;
  bool killRequested = false;
  Mode requestedMode = MODE_MANUAL;
  uint32_t lastCommandUs = 0;
};

struct ControlOutput {
  float steeringNorm = 0.0f;      // -1..1
  float throttleNorm = 0.0f;      // 0..1
  float yawRateTarget = 0.0f;     // deg/s
};

struct FailsafeState {
  bool active = false;
  uint16_t reasonBits = FAIL_NONE;
  uint32_t activatedAtUs = 0;
};

struct BoatState {
  SystemStatus systemStatus = SYS_OK;
  Mode mode = MODE_MANUAL;
  ArmState armState = ARM_DISARMED;
  bool imuReady = false;
  bool baroReady = false;
  bool gpsReady = false;
  uint32_t loopHeartbeatCount = 0;
};

struct PID {
  float kp = 0.0f;
  float ki = 0.0f;
  float kd = 0.0f;
  float iTerm = 0.0f;
  float prevError = 0.0f;
  float prevMeasurement = 0.0f;
  float iLimit = 0.0f;
  float outLimit = 0.0f;
};
