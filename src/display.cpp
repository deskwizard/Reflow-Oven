#include "display.h"

TFT_eSPI lcd = TFT_eSPI();

extern double _ewma;
extern float setpointLow;
extern float setpointHigh;
extern bool tuning;

bool displayUnit = UNIT_C;

uint16_t displayReadRate = 1000;

void initDisplay() {

  lcd.init();
  lcd.setRotation(2);
  lcd.fillScreen(TFT_BLACK);

  lcd.setTextColor(TFT_GREEN);
  lcd.setFreeFont(&FreeSansBold18pt7b);

  lcd.drawLine(0, 25, 127, 25, TFT_SKYBLUE);
  lcd.drawLine(0, 26, 127, 26, TFT_SKYBLUE);

  lcd.drawLine(0, 88, 127, 88, TFT_SKYBLUE);
  lcd.drawLine(0, 89, 127, 89, TFT_SKYBLUE);

  lcd.drawLine(0, 114, 127, 114, TFT_SKYBLUE);
  lcd.drawLine(0, 115, 127, 115, TFT_SKYBLUE);

  // not gonna work because of the VP ?
  // bottom of the power indicator
  lcd.drawLine(0, 159, 157, 159, TFT_GREEN);
  // right side of the power indicator
  lcd.drawLine(127, 137, 127, 159, TFT_GREEN);

  updateUnitDisplay();
  updateSetpointDisplay();
  updatePowerIndicator();
  updateStateIndicator();
}

void updateTuningValues() {
  lcd.setViewport(0, 116, DISPLAY_W, 21);
  lcd.fillScreen(TUNE_VAL_FILL);
  lcd.setFreeFont(&FreeSansBold9pt7b);
  lcd.setTextSize(1);
  lcd.setCursor(0, 15);
  lcd.print(getKp());
  lcd.print(' ');
  lcd.print(getKi());
  lcd.print(' ');
  lcd.print(getKd());
}

void updateStateIndicator() {

  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.fillScreen(IND_STATE_FILL);
  lcd.setFreeFont(&FreeSansBold9pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(CC_DATUM);

  if (tuning) {
    lcd.setTextColor(TFT_ORANGE);
    lcd.drawString("Tuning", DISPLAY_W / 2, 10);
    updateTuningValues();
  } else {
    lcd.drawString("Ready", DISPLAY_W / 2, 10);

    // Wipe the tuning values viewport
    lcd.setViewport(0, 116, DISPLAY_W, 21);
    lcd.fillScreen(TUNE_VAL_FILL);
  }
  /*
    // Setpoint display
  lcd.setViewport(111, SP_VP_Y, 27, SP_H);
  lcd.fillScreen(SP_UNIT_FILL);
  lcd.setFreeFont(&FreeSansBold9pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(TR_DATUM);
  if (displayUnit == UNIT_C) {
    lcd.drawChar('C', 0, 17, GFXFF);

  } else {
    lcd.drawChar('F', 0, 17, GFXFF);
  }
  */
}

void updatePowerIndicator() {

  lcd.setViewport(0, (DISPLAY_H - (IND_PWR_H + 1)), DISPLAY_W, IND_PWR_H);
  lcd.fillScreen(IND_PWR_FILL);

  lcd.drawLine(0, IND_PWR_H, DISPLAY_W, 0, TFT_GREEN);

  uint8_t width = uint8_t((getOutputValue() / 255.0) * DISPLAY_W);
  uint8_t heigth = uint8_t((getOutputValue() / 255.0) * IND_PWR_H);

  // Has a tiny glitch on the leftmost row when at 0... it'll wait.
  lcd.fillTriangle(0, IND_PWR_H, width, IND_PWR_H, width, IND_PWR_H - heigth,
                   TFT_GREEN);
}

void handleDisplay() {

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired

  if ((uint32_t)(currentMillis - previousMillis) >= displayReadRate) {
    updateDisplay();
    updatePowerIndicator();
    if (tuning) {
      // updateTuningValues();
    }
    previousMillis = currentMillis;
  }
}

void updateSetpointDisplay() {
  float displayValue = setpointHigh;
  lcd.setViewport(0, SP_VP_Y, 80, SP_H);
  lcd.fillScreen(SPVAL_FILL);

  if (displayUnit == UNIT_F) {
    displayValue = CtoF(setpointHigh);
  }
  lcd.setFreeFont(&FreeSansBold12pt7b);
  lcd.setTextSize(1);

  // we can move that to another viewport so we don't redraw needlessly
  lcd.setTextDatum(CL_DATUM);
  lcd.drawString("SP: ", 0, 12, GFXFF);

  lcd.setTextDatum(CR_DATUM);
  lcd.drawNumber(displayValue, 80, 12, GFXFF);
}

void updateDisplay() {

  float displayValue = _ewma;

  // lcd.setViewport(0, 26, 110, 60);
  lcd.setViewport(0, TVAL_Y, TVAL_W, TVAL_H);
  lcd.fillScreen(TVAL_FILL);
  lcd.setTextColor(TFT_GREEN);

  if (displayUnit == UNIT_F) {
    displayValue = CtoF(_ewma);
  }
  lcd.setFreeFont(&FreeSansBold18pt7b);
  lcd.setTextSize(2);
  lcd.setTextDatum(CR_DATUM);
  lcd.drawNumber(displayValue, TVAL_W, 28, GFXFF);
}

void toggleDisplayUnit() {
  displayUnit = !displayUnit;
  Serial.print("Unit: ");
  Serial.println(displayUnit);
  updateUnitDisplay();
  updateSetpointDisplay();
}

void updateTuningText() {
  if (tuning) {

  } else {
  }
}

void updateUnitDisplay() {

  // Main display
  lcd.setViewport(T_UNITVP__X, T_UNIT_Y, T_UNIT_W, T_UNIT_H);
  lcd.fillScreen(T_UNIT_FILL);
  lcd.setFreeFont(&FreeSansBold12pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(TR_DATUM);
  if (displayUnit == UNIT_C) {
    lcd.drawChar('C', 0, T_UNIT_Y, GFXFF);

  } else {
    lcd.drawChar('F', 0, T_UNIT_Y, GFXFF);
  }

  // Setpoint display
  lcd.setViewport(81, SP_VP_Y, 27, SP_H);
  lcd.fillScreen(SP_UNIT_FILL);
  lcd.setFreeFont(&FreeSansBold9pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(TR_DATUM);
  if (displayUnit == UNIT_C) {
    lcd.drawChar('C', 0, 17, GFXFF);

  } else {
    lcd.drawChar('F', 0, 17, GFXFF);
  }

  updateDisplay(); // Update the displayed value for the new unit
}