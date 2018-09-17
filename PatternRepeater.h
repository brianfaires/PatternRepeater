#ifndef PATTERN_REPEATER_H
#define PATTERN_REPEATER_H

#include "Arduino.h"
#include "FastLED.h"
#include "PatternCommon.h"
#include "PaletteManager.h"
#include "GammaManager.h"


class PatternRepeater {
  public:
    PatternRepeater();
    virtual void SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs);
    virtual void SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod);
    virtual void SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod);
    virtual void Init(PaletteManager* _pm, GammaManager* gm, uint32_t curTime);
    virtual void Update(uint32_t newTime);
    virtual bool IsReadyForColorMove(uint32_t curTime);
    virtual bool IsReadyForDimMove(uint32_t curTime);
    virtual void SkipTime(uint32_t amount);

    int8_t colorSpeed;
    int8_t dimSpeed;
    uint8_t myBrightness;
    
  private:
    virtual void ScrollColorPattern(bool scrollForward);
    virtual void ScrollDimPattern(bool scrollForward);

    PaletteManager* pm;
    GammaManager* Gamma;
    
    PRGB colorPattern[MAX_PERIOD];
    uint16_t colorPeriod;
    uint16_t colorIndexFirst;
    uint32_t lastColorMove;
    
    uint8_t dimPattern[MAX_PERIOD];
    uint16_t dimPeriod;
    uint16_t dimIndexFirst;
    uint32_t lastDimMove;
};

#endif
