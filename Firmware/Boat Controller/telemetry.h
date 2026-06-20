#pragma once

#include "types.h"

void initTelemetry();
void printBootSummary(const BoatState &state);
void publishTelemetry(
  const BoatState &state,
  const SensorFrame &sensors,
  const EstimatorState &est,
  const CommandInput &cmd,
  const ControlOutput &out,
  const FailsafeState &fail,
  uint32_t nowUs
);