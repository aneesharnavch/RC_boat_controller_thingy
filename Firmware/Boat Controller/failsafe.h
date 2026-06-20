#pragma once

#include "types.h"

void initFailsafe(FailsafeState &fail);
void evaluateFailsafe(
  BoatState &state,
  const SensorFrame &sensors,
  const CommandInput &cmd,
  FailsafeState &fail,
  uint32_t nowUs
);