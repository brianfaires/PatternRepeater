#include "PatternRepeater.h"

PatternRepeater::PatternRepeater() {
  colorPeriod = 1;
  colorPattern[0] = { 0, 0, 0 };
  colorIndexFirst = 0;
  lastColorMove = 0;
  colorSpeed = 0;
  
  dimPeriod = 1;
  dimPattern[0] = 255;
  dimIndexFirst = 0;
  lastDimMove = 0;
  dimSpeed = 0;
  
  myBrightness = 255;
}

void PatternRepeater::SkipTime(uint32_t amount) {
  lastColorMove += amount;
  lastDimMove += amount;
}

void PatternRepeater::SetColorPattern(PRGB* newPattern, uint16_t newColorPeriod) {
  if(colorPeriod != newColorPeriod) { colorIndexFirst = 0; }
  colorPeriod = newColorPeriod;
  memcpy(colorPattern, newPattern, sizeof(PRGB)*colorPeriod);
}

void PatternRepeater::SetDimPattern(uint8_t* newPattern, uint16_t newDimPeriod) {
  if(dimPeriod != newDimPeriod) { dimIndexFirst = 0; }
  dimPeriod = newDimPeriod;
  memcpy(dimPattern, newPattern, dimPeriod);
}

void PatternRepeater::Init(PaletteManager* _pm, GammaManager* gm, uint32_t curTime) {
  Gamma = gm;
  pm = _pm;
  
  colorIndexFirst = 0;
  dimIndexFirst = 0;
  
  lastDimMove = curTime;
  lastColorMove = curTime;
}

void PatternRepeater::Update(uint32_t curTime) {
  // Move brightness pattern
  if(dimSpeed == 0) {
    lastDimMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(dimSpeed);
    if(curTime - lastDimMove >= stepSize) {
      ScrollDimPattern(dimSpeed > 0);
      lastDimMove += ONE_SECOND / abs(dimSpeed);
    }
  }

  // Move color pattern
  if(colorSpeed == 0) {
    lastColorMove = curTime;
  }
  else {
    uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
    if(curTime - lastColorMove >= stepSize) {
      ScrollColorPattern(colorSpeed > 0);
      lastColorMove += ONE_SECOND / abs(colorSpeed);
    }
  }
}
uint8_t curHue = 0;
void PatternRepeater::ScrollColorPattern(bool scrollForward) {
  if(scrollForward) {
	  curHue++;
    if(--colorIndexFirst == 0xFFFF) { colorIndexFirst = colorPeriod - 1; }
  }
  else {
	  curHue--;
    if(++colorIndexFirst == colorPeriod) { colorIndexFirst = 0; }
  }
}

void PatternRepeater::ScrollDimPattern(bool scrollForward) {
  if(scrollForward) {
    if(--dimIndexFirst == 0xFFFF) { dimIndexFirst = dimPeriod - 1; }
  }
  else {
    if(++dimIndexFirst == dimPeriod) { dimIndexFirst = 0; }
  }
}

bool PatternRepeater::IsReadyForDimMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(dimSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(dimSpeed);
  return curTime - lastDimMove >= stepSize;
}

bool PatternRepeater::IsReadyForColorMove(uint32_t curTime) {
  // Returns true if this cycle is going to move the pattern (i.e. only change pattern on the same draw cycle as a move)
  if(colorSpeed == 0) { return true; }
  
  uint32_t stepSize = ONE_SECOND / abs(colorSpeed);
  return curTime - lastColorMove >= stepSize;
}

void PatternRepeater::SetCRGBs(CRGB* target, uint16_t numLEDs) {
  uint16_t curColorIndex = colorIndexFirst;
  uint16_t curDimIndex = dimIndexFirst;

//static uint8_t tempHue = curHue;
//bool doIt = false;
//if(tempHue != curHue) { tempHue = curHue; doIt = true; }
  CRGB tempA, tempB;
  for(uint16_t i = 0; i < numLEDs; i++) {
    // Blend using CRGB to avoid jumping directions around the color wheel
    tempA = pm->palette[colorPattern[curColorIndex].a];
    tempB = pm->palette[colorPattern[curColorIndex].b];

    Gamma->Inverse(tempA);
    Gamma->Inverse(tempB);
    target[i] = blend(tempA, tempB, colorPattern[curColorIndex].blendAmount);

//target[i] = CHSV(curHue, 255, 255);
//Gamma->Inverse(target[i]);
//CRGB tempB = target[i];
	if(dimPattern[curDimIndex] == 0) { target[i] = CRGB::Black; }
    else { Gamma->SetPixel(target[i], max(1, dimPattern[curDimIndex] * myBrightness / 255)); }//debug: += 127?

/*
if(doIt && i >= 10 && i <= 14) {
	Serial.print(String(tempB.r) + "," + String(tempB.g) + "," + String(tempB.b));
	Gamma->Correct(tempB);
	Serial.print(" -> " + String(tempB.r) + "," + String(tempB.g) + "," + String(tempB.b) + "(" + String((uint8_t)(dimPattern[curDimIndex] * myBrightness / 255)) + ")");
	Serial.print(" -> " + String(target[i].r) + "," + String(target[i].g) + "," + String(target[i].b) + "\n");
	if(i == 14) { Serial.println(); }
}*/

    
    #ifdef DEBUG_ERRORS
      if(curDimIndex >= dimPeriod) { Serial.println("ERROR: SetCRGBs(): curDimIndex out of bounds: " + 
      String(curDimIndex) + " / " + String(dimPeriod)); }
      if(curColorIndex >= colorPeriod) { Serial.println("ERROR: SetCRGBs(): curColorIndex out of bounds: " + 
      String(curColorIndex) + " / " + String(colorPeriod)); }
    #endif

    //Serial.println(String(i) + ": " + String(colorPattern[curColorIndex].a) + ", "  + String(colorPattern[curColorIndex].b) + ", "  + String(colorPattern[curColorIndex].blendAmount));
    
    if(++curColorIndex == colorPeriod) { curColorIndex = 0; }
    if(++curDimIndex == dimPeriod) { curDimIndex = 0; }
  }
/*
  Serial.println("SetCRGBs(): ");
  for(uint8_t ii = 0; ii < numLEDs; ii++) {
    Serial.print(String(ii) + " = " + String(dimPattern[ii % dimPeriod]) + "\t");
    Serial.println(String(target[ii].b));
  }*/
}

