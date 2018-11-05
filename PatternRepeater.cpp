#include "PatternRepeater.h"

PatternRepeater::PatternRepeater() {
  colorPeriod = 1;
  colorPattern[0] = { 0, 0, 0 };
  colorIndexFirst = 0;
  
  dimPeriod = 1;
  dimPattern[0] = 255;
  dimIndexFirst = 0;
  
  myBrightness = 255;
}

uint16_t PatternRepeater::GetDimIndexFirst() { return dimIndexFirst; }
uint16_t PatternRepeater::GetColorIndexFirst() { return colorIndexFirst; }

void PatternRepeater::Init(PaletteManager* _pm, GammaManager* gm) {
  if(gm != NULL)  { Gamma = gm; }
  if(_pm != NULL) { pm = _pm; }
  
  colorIndexFirst = 0;
  dimIndexFirst = 0;
}

void PatternRepeater::SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod) {
  if(dimPeriod != newDimPeriod) { dimIndexFirst = 0; }
  dimPeriod = newDimPeriod;
  memcpy(dimPattern, newPattern, dimPeriod);
}

void PatternRepeater::SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod) {
  if(colorPeriod != newColorPeriod) { colorIndexFirst = 0; }
  colorPeriod = newColorPeriod;
  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPeriod);
}

void PatternRepeater::ScrollDimPattern(bool scrollForward) {
  if(scrollForward) {
    if(--dimIndexFirst == 0xFF) { dimIndexFirst = dimPeriod - 1; }
  }
  else {
    if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
  }
}

void PatternRepeater::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
    if(--colorIndexFirst == 0xFF) { colorIndexFirst = colorPeriod - 1; }
  }
  else {
    if(++colorIndexFirst == colorPeriod) { colorIndexFirst = 0; }
  }
}

void PatternRepeater::SetCRGBs(CRGB* target, uint8_t* target_b, uint16_t numLEDs) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curDimIndex = dimIndexFirst;

  CRGB tempA, tempB;
  for(uint16_t i = 0; i < numLEDs; i++) {
    // Blend using CRGB to avoid jumping directions around the color wheel
    tempA = pm->palette[colorPattern[curColorIndex].a];
    tempB = pm->palette[colorPattern[curColorIndex].b];

    Gamma->Inverse(tempA);
    Gamma->Inverse(tempB);
    target[i] = blend(tempA, tempB, colorPattern[curColorIndex].blendAmount);

	if(dimPattern[curDimIndex] == 0) { target_b[i] = 0; }
    else {
		Gamma->Correct(target[i]);
		uint16_t brightness = (uint16_t)dimPattern[curDimIndex] * (uint16_t)(myBrightness+1) / 0x100;
		target_b[i] = max(1, brightness & 0xFF); //debug: += 127?
	}

      if(curDimIndex >= dimPeriod) { THROW("ERROR: SetCRGBs(): curDimIndex out of bounds: " + String(curDimIndex) + " / " + String(dimPeriod)) }
      if(curColorIndex >= colorPeriod) { THROW("ERROR: SetCRGBs(): curColorIndex out of bounds: " + String(curColorIndex) + " / " + String(colorPeriod)) }

    if(++curColorIndex == colorPeriod) { curColorIndex = 0; }
    if(++curDimIndex == dimPeriod) { curDimIndex = 0; }
  }
}

