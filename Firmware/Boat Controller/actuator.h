#pragma once

#include "types.h"

bool initActuators(BoatState &state);
void applyActuatorOutputs(const ControlOutput &out, BoatState &state, const FailsafeState &fail, uint32_t nowUs);

