#include "control.h"
#include "config.h"

static PID pidHeading;
static PID pidYawRate;
static PID pidSpeed;

static float clampf(float v, float lo, float hi) {
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static float wrapDeg(float a) {
  while (a > 180.0f) a -= 360.0f;
  while (a < -180.0f) a += 360.0f;
  return a;
}

static void initPid(PID &p, float kp, float ki, float kd, float iLim, float outLim) {
  p.kp = kp;
  p.ki = ki;
  p.kd = kd;
  p.iTerm = 0.0f;
  p.prevError = 0.0f;
  p.prevMeasurement = 0.0f;
  p.iLimit = iLim;
  p.outLimit = outLim;
}

static float stepPid(PID &p, float setpoint, float measurement, float dt, bool derivativeOnMeasurement = true) {
  float error = setpoint - measurement;
  p.iTerm += error * p.ki * dt;
  p.iTerm = clampf(p.iTerm, -p.iLimit, p.iLimit);

  float dTerm = 0.0f;
  if (dt > 1e-5f) {
    if (derivativeOnMeasurement) {
      dTerm = -p.kd * (measurement - p.prevMeasurement) / dt;
    } else {
      dTerm = p.kd * (error - p.prevError) / dt;
    }
  }

  float out = p.kp * error + p.iTerm + dTerm;
  out = clampf(out, -p.outLimit, p.outLimit);

  p.prevError = error;
  p.prevMeasurement = measurement;
  return out;
}

void initDefaults(BoatState &state, CommandInput &cmd, ControlOutput &out, FailsafeState &fail) {
  state.mode = MODE_MANUAL;
  state.systemStatus = SYS_OK;
  state.armState = ARM_DISARMED;

  cmd.throttle = 0.0f;
  cmd.steering = 0.0f;
  cmd.targetHeadingDeg = 0.0f;
  cmd.targetSpeedMps = 0.0f;
  cmd.armRequested = false;
  cmd.killRequested = false;
  cmd.requestedMode = MODE_MANUAL;
  cmd.lastCommandUs = micros();

  out.steeringNorm = 0.0f;
  out.throttleNorm = 0.0f;
  out.yawRateTarget = 0.0f;

  fail.active = false;
  fail.reasonBits = FAIL_NONE;
  fail.activatedAtUs = 0;
}

void initController(BoatState &) {
  initPid(pidHeading, PID_HEADING_KP, PID_HEADING_KI, PID_HEADING_KD, PID_HEADING_I_LIM, PID_HEADING_OUT_LIM);
  initPid(pidYawRate, PID_YAWRATE_KP, PID_YAWRATE_KI, PID_YAWRATE_KD, PID_YAWRATE_I_LIM, PID_YAWRATE_OUT_LIM);
  initPid(pidSpeed, PID_SPEED_KP, PID_SPEED_KI, PID_SPEED_KD, PID_SPEED_I_LIM, PID_SPEED_OUT_LIM);
}

void computeControl(
  BoatState &state,
  const SensorFrame &sensors,
  const EstimatorState &est,
  CommandInput &cmd,
  const FailsafeState &fail,
  ControlOutput &out,
  float dt
) {
  state.mode = cmd.requestedMode;

  if (fail.active || cmd.killRequested) {
    out.steeringNorm = 0.0f;
    out.throttleNorm = 0.0f;
    out.yawRateTarget = 0.0f;
    return;
  }

  float steerManual = clampf(cmd.steering, -1.0f, 1.0f);
  float thrManual = clampf(cmd.throttle, 0.0f, 1.0f);

  float steerCmd = steerManual;
  float thrCmd = thrManual;
  float yawRateTarget = 0.0f;

  if (state.mode == MODE_ASSISTED && ENABLE_RATE_DAMPING) {
    yawRateTarget = steerManual * 30.0f;
    float yawRateCorrection = stepPid(pidYawRate, yawRateTarget, est.yawRateDps, dt, true);
    steerCmd = clampf(steerManual + yawRateCorrection, -1.0f, 1.0f);
  }

  if (state.mode == MODE_HEADING_HOLD && ENABLE_HEADING_HOLD) {
    float headingError = wrapDeg(cmd.targetHeadingDeg - est.yawDeg);
    yawRateTarget = stepPid(pidHeading, 0.0f, -headingError, dt, false);
    float steerFromYaw = stepPid(pidYawRate, yawRateTarget, est.yawRateDps, dt, true);
    steerCmd = clampf(steerFromYaw, -1.0f, 1.0f);
  }

  if (state.mode == MODE_COURSE_HOLD && ENABLE_HEADING_HOLD) {
    float courseHeading = sensors.gps.valid ? sensors.gps.courseDeg : est.yawDeg;
    float headingError = wrapDeg(courseHeading - est.yawDeg);
    yawRateTarget = stepPid(pidHeading, 0.0f, -headingError, dt, false);
    steerCmd = clampf(stepPid(pidYawRate, yawRateTarget, est.yawRateDps, dt, true), -1.0f, 1.0f);
  }

  if (ENABLE_SPEED_HOLD && cmd.targetSpeedMps > 0.1f && sensors.gps.valid && sensors.gps.hasFix) {
    float speedTrim = stepPid(pidSpeed, cmd.targetSpeedMps, est.fusedSpeedMps, dt, false);
    thrCmd = clampf(thrManual + speedTrim, 0.0f, 1.0f);
  }

  if (fabsf(est.rollDeg) > MAX_ABS_ROLL_DEG_FOR_FULL_THR || fabsf(est.pitchDeg) > MAX_ABS_PITCH_DEG_FOR_FULL_THR) {
    thrCmd *= FAILSAFE_THROTTLE_FACTOR;
  }

  out.steeringNorm = steerCmd;
  out.throttleNorm = thrCmd;
  out.yawRateTarget = yawRateTarget;
}