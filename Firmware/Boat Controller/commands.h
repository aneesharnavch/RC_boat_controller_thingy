#pragma once

#include "types.h"

void initCommands();
void pollCommands(BoatState &state, CommandInput &cmd, uint32_t nowUs);