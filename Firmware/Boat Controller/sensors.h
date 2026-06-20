#pragma once

#include "types.h"

bool initSensors(BoatState &state);
void updateImu(SensorFrame &frame, BoatState &state, uint32_t nowUs);
void updateBaro(SensorFrame &frame, BoatState &state, uint32_t nowUs);
void updateGps(SensorFrame &frame, BoatState &state, uint32_t nowUs);
