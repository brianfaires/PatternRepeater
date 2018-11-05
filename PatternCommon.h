#pragma once
#include "Arduino.h"
#include "ArduinoTrace.h"
#include "FastLED.h"

#define ONE_SECOND 1000000
#define MAX_PERIOD 210

struct PRGB {
  uint8_t a;
  uint8_t b;
  uint8_t blendAmount;
};
