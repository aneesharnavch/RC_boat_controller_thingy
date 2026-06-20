#pragma once

#include "types.h"

void initEstimator(EstimatorState &est);
void updateEstimatorFromImu(const SensorFrame &frame, EstimatorState &est, BoatState &state, float dt);
void updateEstimatorFromBaro(const SensorFrame &frame, EstimatorState &est, BoatState &state);
void updateEstimatorFromGps(const SensorFrame &frame, EstimatorState &est, BoatState &state);