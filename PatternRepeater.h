#ifndef PATTERN_REPEATER_H
#define PATTERN_REPEATER_H

#include "Arduino.h"
#include "FastLED.h"
#include "PatternCommon.h"
#include "PaletteManager.h"
#include "GammaManager.h"
#include "ArduinoTrace.h"


class PatternRepeater {
  public:
    PatternRepeater();
    void Init(PaletteManager* _pm, GammaManager* gm);
    void SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod);
    void SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod);
    void ScrollDimPattern(bool scrollForward);
    void ScrollColorPattern(bool scrollForward);
    void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
	uint16_t GetDimIndexFirst();
	uint16_t GetColorIndexFirst();

    uint8_t myBrightness;
    
  private:

    PaletteManager* pm;
    GammaManager* Gamma;
    
    PRGB colorPattern[MAX_PERIOD];
    uint8_t colorPeriod;
    uint8_t colorIndexFirst;
    
    uint8_t dimPattern[MAX_PERIOD];
    uint8_t dimPeriod;
    uint8_t dimIndexFirst;
};

#endif
