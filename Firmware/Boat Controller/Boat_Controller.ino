#include "config.h"
#include "types.h"
#include "sensors.h"
#include "estimator.h"
#include "actuator.h"
#include "control.h"
#include "failsafe.h"
#include "telemetry.h"
#include "commands.h"
#include "radio.h"

BoatState gState = {};
SensorFrame gSensors = {};
EstimatorState gEst = {};
CommandInput gCmd = {};
ControlOutput gOut = {};
FailsafeState gFail = {};

static uint32_t lastImuUs = 0;
static uint32_t lastBaroUs = 0;
static uint32_t lastGpsUs = 0;
static uint32_t lastCtlUs = 0;
static uint32_t lastTelUs = 0;
static uint32_t lastCmdUs = 0;
static uint32_t lastHeartbeatUs = 0;

static void runImuTask(uint32_t nowUs) {
  float dt = (lastImuUs == 0) ? (1.0f / IMU_HZ) : ((nowUs - lastImuUs) * 1e-6f);
  lastImuUs = nowUs;

  updateImu(gSensors, gState, nowUs);
  updateEstimatorFromImu(gSensors, gEst, gState, dt);
}

static void runBaroTask(uint32_t nowUs) {
  lastBaroUs = nowUs;
  updateBaro(gSensors, gState, nowUs);
  updateEstimatorFromBaro(gSensors, gEst, gState);
}

static void runGpsTask(uint32_t nowUs) {
  lastGpsUs = nowUs;
  updateGps(gSensors, gState, nowUs);
  updateEstimatorFromGps(gSensors, gEst, gState);
}

static void runControlTask(uint32_t nowUs) {
  float dt = (lastCtlUs == 0) ? (1.0f / CONTROL_HZ) : ((nowUs - lastCtlUs) * 1e-6f);
  lastCtlUs = nowUs;

  evaluateFailsafe(gState, gSensors, gCmd, gFail, nowUs);
  computeControl(gState, gSensors, gEst, gCmd, gFail, gOut, dt);
  applyActuatorOutputs(gOut, gState, gFail, nowUs);
}

static void runTelemetryTask(uint32_t nowUs) {
  lastTelUs = nowUs;
  publishTelemetry(gState, gSensors, gEst, gCmd, gOut, gFail, nowUs);
}

static void runCommandTask(uint32_t nowUs) {
  lastCmdUs = nowUs;
  pollCommands(gState, gCmd, nowUs);
  if (USE_NRF24) {
    pollRadio(gState, gCmd, nowUs);
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(BOOT_DELAY_MS);

  initDefaults(gState, gCmd, gOut, gFail);
  initTelemetry();
  initCommands();

  bool sensorsOk = initSensors(gState);
  bool actuatorsOk = initActuators(gState);
  initEstimator(gEst);
  initController(gState);
  initFailsafe(gFail);

  bool radioOk = !USE_NRF24 || initRadio();

  if (!sensorsOk || !actuatorsOk || !radioOk) {
    gState.systemStatus = SYS_DEGRADED;
  }

  printBootSummary(gState);
}

void loop() {
  uint32_t nowUs = micros();

  if (nowUs - lastCmdUs >= CMD_PERIOD_US) {
    runCommandTask(nowUs);
  }
  if (nowUs - lastImuUs >= IMU_PERIOD_US) {
    runImuTask(nowUs);
  }
  if (nowUs - lastBaroUs >= BARO_PERIOD_US) {
    runBaroTask(nowUs);
  }
  if (nowUs - lastGpsUs >= GPS_PERIOD_US) {
    runGpsTask(nowUs);
  }
  if (nowUs - lastCtlUs >= CONTROL_PERIOD_US) {
    runControlTask(nowUs);
  }
  if (nowUs - lastTelUs >= TELEMETRY_PERIOD_US) {
    runTelemetryTask(nowUs);
  }

  if (nowUs - lastHeartbeatUs >= HEARTBEAT_PERIOD_US) {
    lastHeartbeatUs = nowUs;
    gState.loopHeartbeatCount++;
  }
}
