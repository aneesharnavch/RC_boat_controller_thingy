#include "telemetry.h"

#include <Arduino.h>

void initTelemetry() {}

void printBootSummary(const BoatState &state) {
  Serial.println(F("BOOT: Boat Controller"));
  Serial.print(F("IMU ready: ")); Serial.println(state.imuReady ? F("yes") : F("no"));
  Serial.print(F("BARO ready: ")); Serial.println(state.baroReady ? F("yes") : F("no"));
  Serial.print(F("GPS ready: ")); Serial.println(state.gpsReady ? F("yes") : F("no"));
}

void publishTelemetry(
  const BoatState &state,
  const SensorFrame &sensors,
  const EstimatorState &est,
  const CommandInput &cmd,
  const ControlOutput &out,
  const FailsafeState &fail,
  uint32_t nowUs
) {
  Serial.print(F("t_us=")); Serial.print(nowUs);
  Serial.print(F(",mode=")); Serial.print(static_cast<int>(state.mode));
  Serial.print(F(",arm=")); Serial.print(static_cast<int>(state.armState));
  Serial.print(F(",fs=")); Serial.print(fail.active ? 1 : 0);
  Serial.print(F(",fs_bits=")); Serial.print(fail.reasonBits);

  Serial.print(F(",roll=")); Serial.print(est.rollDeg, 2);
  Serial.print(F(",pitch=")); Serial.print(est.pitchDeg, 2);
  Serial.print(F(",yaw=")); Serial.print(est.yawDeg, 2);
  Serial.print(F(",yaw_rate=")); Serial.print(est.yawRateDps, 2);

  Serial.print(F(",thr_in=")); Serial.print(cmd.throttle, 3);
  Serial.print(F(",str_in=")); Serial.print(cmd.steering, 3);
  Serial.print(F(",thr_out=")); Serial.print(out.throttleNorm, 3);
  Serial.print(F(",str_out=")); Serial.print(out.steeringNorm, 3);

  Serial.print(F(",gps_fix=")); Serial.print(sensors.gps.hasFix ? 1 : 0);
  Serial.print(F(",gps_spd=")); Serial.print(sensors.gps.speedMps, 2);
  Serial.print(F(",gps_crs=")); Serial.print(sensors.gps.courseDeg, 2);
  Serial.print(F(",sats=")); Serial.print(sensors.gps.sats);
  Serial.print(F(",hdop=")); Serial.print(sensors.gps.hdop, 1);

  Serial.print(F(",baro_p=")); Serial.print(sensors.baro.pressurePa, 1);
  Serial.print(F(",baro_alt=")); Serial.print(sensors.baro.altitudeM, 2);
  Serial.println();
}