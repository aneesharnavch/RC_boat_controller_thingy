#include "actuator.h"
#include <Servo.h>
#include "config.h"

static Servo sRudder;
static Servo sEsc;

static int sServoUs = SERVO_US_CENTER;
static int sEscUs = ESC_US_MIN;
static uint32_t sArmStartedMs = 0;

static int clampi(int v, int lo, int hi) {
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static float clampf(float v, float lo, float hi) {
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static int slewStep(int current, int target, int maxDelta) {
  int delta = target - current;
  if (delta > maxDelta) return current + maxDelta;
  if (delta < -maxDelta) return current - maxDelta;
  return target;
}

bool initActuators(BoatState &state) {
  sRudder.attach(PIN_SERVO_SIGNAL, SERVO_US_MIN, SERVO_US_MAX);
  sEsc.attach(PIN_ESC_SIGNAL, ESC_US_MIN, ESC_US_MAX);

  sServoUs = SERVO_US_CENTER;
  sEscUs = ESC_US_MIN;
  sRudder.writeMicroseconds(sServoUs);
  sEsc.writeMicroseconds(sEscUs);

  state.armState = ARM_DISARMED;
  return true;
}

void applyActuatorOutputs(const ControlOutput &out, BoatState &state, const FailsafeState &fail, uint32_t nowUs) {
  float steer = clampf(out.steeringNorm, -1.0f, 1.0f);
  float throttle = clampf(out.throttleNorm, 0.0f, 1.0f);

  if (fail.active) {
    steer = 0.0f;
    throttle = 0.0f;
    state.armState = ARM_DISARMED;
  }

  if (state.armState == ARM_DISARMED) {
    if (throttle <= 0.01f) {
      state.armState = ARM_ARMING;
      sArmStartedMs = millis();
    } else {
      throttle = 0.0f;
    }
  }

  if (state.armState == ARM_ARMING) {
    throttle = 0.0f;
    if ((millis() - sArmStartedMs) >= ESC_ARM_TIME_MS) {
      state.armState = ARM_ARMED;
    }
  }

  int targetServo = SERVO_US_CENTER + static_cast<int>(steer * SERVO_MAX_DEFLECT_US);
  targetServo = clampi(targetServo, SERVO_US_MIN, SERVO_US_MAX);

  int targetEsc = ESC_US_IDLE + static_cast<int>(throttle * (ESC_US_MAX - ESC_US_IDLE));
  targetEsc = clampi(targetEsc, ESC_US_MIN, ESC_US_MAX);
  if (state.armState != ARM_ARMED) {
    targetEsc = ESC_ARM_US;
  }

  sServoUs = slewStep(sServoUs, targetServo, SERVO_MAX_SLEW_US_PER_TICK);
  sEscUs = slewStep(sEscUs, targetEsc, ESC_MAX_SLEW_US_PER_TICK);

  sRudder.writeMicroseconds(sServoUs);
  sEsc.writeMicroseconds(sEscUs);

  (void)nowUs;
}
