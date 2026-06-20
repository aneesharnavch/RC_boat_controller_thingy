#include "estimator.h"
#include "config.h"

static float wrapDeg(float a) {
  while (a > 180.0f) a -= 360.0f;
  while (a < -180.0f) a += 360.0f;
  return a;
}

void initEstimator(EstimatorState &est) {
  est.rollDeg = 0.0f;
  est.pitchDeg = 0.0f;
  est.yawDeg = 0.0f;
  est.yawRateDps = 0.0f;
  est.fusedSpeedMps = 0.0f;
  est.filteredAltM = 0.0f;
}

void updateEstimatorFromImu(const SensorFrame &frame, EstimatorState &est, BoatState &state, float dt) {
  if (!frame.imu.valid || dt <= 0.0f || dt > 0.2f) {
    return;
  }

  float ax = frame.imu.ax;
  float ay = frame.imu.ay;
  float az = frame.imu.az;
  float gx = frame.imu.gx;
  float gy = frame.imu.gy;
  float gz = frame.imu.gz;

  float accelRoll = atan2f(ay, az) * 57.2957795f;
  float accelPitch = atan2f(-ax, sqrtf(ay * ay + az * az)) * 57.2957795f;

  est.rollDeg = COMPLEMENTARY_ALPHA * (est.rollDeg + gx * dt) + (1.0f - COMPLEMENTARY_ALPHA) * accelRoll;
  est.pitchDeg = COMPLEMENTARY_ALPHA * (est.pitchDeg + gy * dt) + (1.0f - COMPLEMENTARY_ALPHA) * accelPitch;
  est.yawDeg = wrapDeg(est.yawDeg + gz * dt);
  est.yawRateDps = 0.82f * est.yawRateDps + 0.18f * gz;

  state.systemStatus = (state.systemStatus == SYS_FAILSAFE) ? SYS_FAILSAFE : SYS_OK;
}

void updateEstimatorFromBaro(const SensorFrame &frame, EstimatorState &est, BoatState &) {
  if (!frame.baro.valid) {
    return;
  }
  est.filteredAltM = 0.88f * est.filteredAltM + 0.12f * frame.baro.altitudeM;
}

void updateEstimatorFromGps(const SensorFrame &frame, EstimatorState &est, BoatState &) {
  if (!frame.gps.valid || !frame.gps.hasFix) {
    est.fusedSpeedMps *= 0.995f;
    return;
  }
  est.fusedSpeedMps = 0.85f * est.fusedSpeedMps + 0.15f * frame.gps.speedMps;
}