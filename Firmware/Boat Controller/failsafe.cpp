#include "failsafe.h"
#include "config.h"

void initFailsafe(FailsafeState &fail) {
  fail.active = false;
  fail.reasonBits = FAIL_NONE;
  fail.activatedAtUs = 0;
}

void evaluateFailsafe(
  BoatState &state,
  const SensorFrame &sensors,
  const CommandInput &cmd,
  FailsafeState &fail,
  uint32_t nowUs
) {
  uint16_t reasons = FAIL_NONE;

  // Stale IMU data trips the timeout independently of the valid flag. If the
  // sensor was reading fine and then hangs mid-run, lastUpdateUs stops advancing
  // and this catches it; the old nested form could only ever fire on a boot
  // failure (valid is never cleared at runtime).
  if (nowUs - sensors.imu.lastUpdateUs > SENSOR_TIMEOUT_IMU_US) {
    reasons |= FAIL_IMU_TIMEOUT;
  }
  if (!sensors.imu.valid) {
    reasons |= FAIL_IMU_INVALID;
  }
  if (nowUs - cmd.lastCommandUs > COMMAND_TIMEOUT_US) {
    reasons |= FAIL_CMD_TIMEOUT;
  }
  if (cmd.killRequested) {
    reasons |= FAIL_MANUAL_KILL;
  }

  fail.reasonBits = reasons;
  fail.active = (reasons != FAIL_NONE);
  if (fail.active && fail.activatedAtUs == 0) {
    fail.activatedAtUs = nowUs;
  }
  if (!fail.active) {
    fail.activatedAtUs = 0;
  }

  state.systemStatus = fail.active ? SYS_FAILSAFE : SYS_OK;
}
