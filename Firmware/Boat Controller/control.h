#pragma once

#include "types.h"

void initDefaults(BoatState &state, CommandInput &cmd, ControlOutput &out, FailsafeState &fail);
void initController(BoatState &state);
void computeControl(
  BoatState &state,
  const SensorFrame &sensors,
  const EstimatorState &est,
  CommandInput &cmd,
  const FailsafeState &fail,
  ControlOutput &out,
  float dt
);
