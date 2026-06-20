#include "commands.h"

#include <Arduino.h>
#include <string.h>
#include <stdlib.h>
#include "config.h"

static char cmdBuffer[CMD_BUFFER_LEN];
static size_t cmdLen = 0;

static float clampf(float v, float lo, float hi) {
  return (v < lo) ? lo : ((v > hi) ? hi : v);
}

static void parseMode(const char *arg, CommandInput &cmd) {
  if (strcmp(arg, "manual") == 0) cmd.requestedMode = MODE_MANUAL;
  else if (strcmp(arg, "assist") == 0) cmd.requestedMode = MODE_ASSISTED;
  else if (strcmp(arg, "head") == 0) cmd.requestedMode = MODE_HEADING_HOLD;
  else if (strcmp(arg, "course") == 0) cmd.requestedMode = MODE_COURSE_HOLD;
}

static void executeLine(char *line, BoatState &state, CommandInput &cmd, uint32_t nowUs) {
  char *tok = strtok(line, " ,\t\r\n");
  if (!tok) return;

  if (strcmp(tok, "arm") == 0) {
    cmd.armRequested = true;
    cmd.killRequested = false;
  } else if (strcmp(tok, "disarm") == 0) {
    cmd.armRequested = false;
    cmd.killRequested = true;
    state.armState = ARM_DISARMED;
  } else if (strcmp(tok, "kill") == 0) {
    cmd.killRequested = true;
  } else if (strcmp(tok, "throttle") == 0) {
    char *v = strtok(nullptr, " ,\t\r\n");
    if (v) cmd.throttle = clampf(static_cast<float>(atof(v)), 0.0f, 1.0f);
  } else if (strcmp(tok, "steer") == 0) {
    char *v = strtok(nullptr, " ,\t\r\n");
    if (v) cmd.steering = clampf(static_cast<float>(atof(v)), -1.0f, 1.0f);
  } else if (strcmp(tok, "heading") == 0) {
    char *v = strtok(nullptr, " ,\t\r\n");
    if (v) cmd.targetHeadingDeg = static_cast<float>(atof(v));
  } else if (strcmp(tok, "speed") == 0) {
    char *v = strtok(nullptr, " ,\t\r\n");
    if (v) cmd.targetSpeedMps = clampf(static_cast<float>(atof(v)), 0.0f, 30.0f);
  } else if (strcmp(tok, "mode") == 0) {
    char *v = strtok(nullptr, " ,\t\r\n");
    if (v) parseMode(v, cmd);
  } else if (strcmp(tok, "help") == 0) {
    Serial.println(F("CMD: arm|disarm|kill|throttle X|steer X|heading X|speed X|mode manual|assist|head|course"));
  }

  cmd.lastCommandUs = nowUs;
}

void initCommands() {
  cmdLen = 0;
  memset(cmdBuffer, 0, sizeof(cmdBuffer));
}

void pollCommands(BoatState &state, CommandInput &cmd, uint32_t nowUs) {
  while (Serial.available() > 0) {
    char c = static_cast<char>(Serial.read());
    if (c == CMD_DELIM) {
      cmdBuffer[cmdLen] = '\0';
      executeLine(cmdBuffer, state, cmd, nowUs);
      cmdLen = 0;
      memset(cmdBuffer, 0, sizeof(cmdBuffer));
    } else if (cmdLen < (CMD_BUFFER_LEN - 1)) {
      cmdBuffer[cmdLen++] = c;
    } else {
      cmdLen = 0;
    }
  }
}
