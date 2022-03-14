#include "display.h"

// Image to byte array https://javl.github.io/image2cpp/
// Choose "Horizontal, 1 bit per pixel" for monochrome
#include "sprites.h"

TFT_eSPI lcd = TFT_eSPI();
// Declare Sprite object "spr_fan" with pointer to "lcd" object
TFT_eSprite spr_fan = TFT_eSprite(&lcd);

bool displayUnit = UNIT_C;
uint16_t displayReadRate = 1000;

bool getDisplayUnit() { return displayUnit; }

void initSprite() {
  // Create a sprite of defined size and colour depth
  spr_fan.createSprite(SPR_FAN_WIDTH, SPR_FAN_HEIGHT);
  spr_fan.setColorDepth(1);
  spr_fan.setBitmapColor(TFT_DARKGREY, TFT_BLACK);

  // Push the image to the sprite - only need to do this once.
  spr_fan.pushImage(
      SPR_FAN_BORDER, SPR_FAN_BORDER, SPR_FAN_WIDTH - (SPR_FAN_BORDER / 2),
      SPR_FAN_HEIGHT - (SPR_FAN_BORDER / 2), (uint16_t *)fan_icon);

  // Set the TFT pivot point to the centre of the sprite
  lcd.setPivot(SPR_FAN_X_POS + SPR_FAN_X_PIV, SPR_FAN_Y_POS + SPR_FAN_Y_PIV);

  // Set Sprite pivot point to centre of Sprite
  spr_fan.setPivot(SPR_FAN_X_PIV, SPR_FAN_Y_PIV);

  lcd.setViewport(SPR_FAN_X_POS, SPR_FAN_Y_POS, SPR_FAN_WIDTH, SPR_FAN_HEIGHT);
  lcd.fillScreen(SPR_FAN_FILL);
  spr_fan.pushSprite(0, 0); // Push sprite at VP origin
}

void updateFanStateDisplay(bool state) {
  if (state) {
    spr_fan.setBitmapColor(TFT_WHITE, TFT_BLACK);
  } else {
    spr_fan.setBitmapColor(TFT_DARKGREY, TFT_BLACK);
    // spr_fan.pushRotated(0); // makes it looks odd
  }
}

void updateFanSprite() {

  if (getFanState() == false) {
    return;
  }

  static uint16_t angle = 0;

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired

  if ((uint32_t)(currentMillis - previousMillis) >= SPR_FAN_UPDATE_RATE) {
    lcd.setViewport(SPR_FAN_X_POS, SPR_FAN_Y_POS, SPR_FAN_WIDTH,
                    SPR_FAN_HEIGHT);
    spr_fan.pushRotated(angle);

    angle = angle + SPR_FAN_ANGLE_PER_FRAME;

    if (angle > 360) {
      angle = 0;
    }
    previousMillis = currentMillis;
  }
}

void initDisplay() {

  lcd.init();
  lcd.setRotation(2);
  lcd.fillScreen(TFT_BLACK);
  /******************************************************/

  /*****************************************************/

  lcd.setTextColor(TFT_GREEN);
  lcd.setFreeFont(&FreeSansBold18pt7b);

  lcd.drawLine(0, 25, 127, 25, TFT_SKYBLUE);
  lcd.drawLine(0, 26, 127, 26, TFT_SKYBLUE);

  lcd.drawLine(0, 88, 127, 88, TFT_SKYBLUE);
  lcd.drawLine(0, 89, 127, 89, TFT_SKYBLUE);

  lcd.drawLine(0, 114, 127, 114, TFT_SKYBLUE);
  lcd.drawLine(0, 115, 127, 115, TFT_SKYBLUE);

  if (getDeviceMode() != MODE_OTA) {

    lcd.drawRect(0, (DISPLAY_H - IND_PWR_H) - 2, DISPLAY_W, IND_PWR_H + 2,
                 TFT_GREEN);

    initSprite();

    updateUnitDisplay();
    updateSetpointDisplay();
    updatePowerIndicator();
    updateStateIndicator();
  }
}

void updatePIDValues() {
  lcd.setViewport(0, 116, DISPLAY_W, 21);
  lcd.fillScreen(TUNE_VAL_FILL);

  if (getDeviceMode() == MODE_PID_DWELL) {
    // We just wipe the screeen
    return;
  }

  lcd.setFreeFont(&FreeSans9pt7b);
  lcd.setTextSize(1);
  lcd.setCursor(0, 15);

  lcd.print(getOutputValue(), 0);
  lcd.print("/");
  lcd.print(getSpan(), 0);
  lcd.print(" (");
  lcd.print((getOutputValue() / getSpan()) * 100, 0);
  lcd.print("%)");

  // lcd.print(getKp());
  // lcd.print(' ');
  // lcd.print(getKi());
  // lcd.print(' ');
  // lcd.print(getKd());
}

void updateStateIndicator() {

  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.fillScreen(IND_STATE_FILL);
  lcd.setFreeFont(&FreeSansBold9pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(CC_DATUM);

  if (getDeviceMode() == MODE_PID_RUNNING) {
    lcd.setTextColor(TFT_BLUE);
    lcd.drawString("Running", DISPLAY_W / 2, 10);
    updatePIDValues();
  } else if (getDeviceMode() == MODE_PID_PREHEAT) {
    lcd.setTextColor(TFT_ORANGE);
    lcd.drawString("Preheat", DISPLAY_W / 2, 10);
    updatePIDValues();
  } else if (getDeviceMode() == MODE_PID_DWELL) {
    lcd.setTextColor(TFT_YELLOW);
    lcd.drawString("Dwell", DISPLAY_W / 2, 10);
  }

  else {
    lcd.drawString("Ready", DISPLAY_W / 2, 10);

    // Wipe the tuning values viewport
    lcd.setViewport(0, 116, DISPLAY_W, 21);
    lcd.fillScreen(TUNE_VAL_FILL);
  }
}

void updatePowerIndicator() {

  lcd.setViewport(1, (DISPLAY_H - (IND_PWR_H + 1)), DISPLAY_W - 2, IND_PWR_H);
  lcd.fillScreen(IND_PWR_FILL);

  uint8_t width = uint8_t((getOutputValue() / MAX_DUTY_CYCLE) * DISPLAY_W);

  lcd.fillRect(0, 0, width, IND_PWR_H, TFT_GREEN);
}

void handleDisplay() {

  uint32_t currentMillis = millis();  // Get snapshot of time
  static uint32_t previousMillis = 0; // Tracks the time since last event fired

  if ((uint32_t)(currentMillis - previousMillis) >= displayReadRate) {
    updateDisplay();
    updatePowerIndicator();
    if (getDeviceMode() == MODE_PID_RUNNING) {
      updatePIDValues();
    }
    previousMillis = currentMillis;
  }

  updateFanSprite();
}

void updateSetpointDisplay() {
  float displayValue = getSetpointHigh();
  lcd.setViewport(0, SP_VP_Y, SP_VP_W, SP_H);
  lcd.fillScreen(SPVAL_FILL);

  if (displayUnit == UNIT_F) {
    displayValue = CtoF(displayValue);
  }
  lcd.setFreeFont(&FreeSans12pt7b);
  lcd.setTextSize(1);

  lcd.setTextDatum(CR_DATUM);
  lcd.drawNumber(uint16_t(displayValue), SP_VP_W, 12, GFXFF);
}

void updateDisplay() {

  float displayValue = getAverageTemperature();

  // lcd.setViewport(0, 26, 110, 60);
  lcd.setViewport(0, TVAL_Y, TVAL_W, TVAL_H);
  lcd.fillScreen(TVAL_FILL);
  lcd.setTextColor(TFT_GREEN);

  if (displayUnit == UNIT_F) {
    displayValue = CtoF(displayValue);
  }
  lcd.setFreeFont(&FreeSansBold18pt7b);
  lcd.setTextSize(2);
  lcd.setTextDatum(CR_DATUM);

  lcd.drawFloat(displayValue, 0, TVAL_W, 28, GFXFF);
}

void toggleDisplayUnit() {
  displayUnit = !displayUnit;
  Serial.print("Unit: ");
  Serial.println(displayUnit);
  updateUnitDisplay();
  updateSetpointDisplay();
}

void updateUnitDisplay() {

  lcd.setTextColor(TFT_GREEN);

  // Main display
  lcd.setViewport(T_UNITVP__X, T_UNIT_Y, T_UNIT_W, T_UNIT_H);
  lcd.fillScreen(T_UNIT_FILL);
  lcd.setTextColor(TFT_GREEN);
  lcd.setFreeFont(&FreeSansBold12pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(TR_DATUM);
  if (displayUnit == UNIT_C) {
    lcd.drawChar('C', 0, T_UNIT_Y, GFXFF);

  } else {
    lcd.drawChar('F', 0, T_UNIT_Y, GFXFF);
  }

  // Setpoint display
  lcd.setViewport(SP_VP_W, SP_VP_Y, 18, SP_H);
  lcd.fillScreen(SP_UNIT_FILL);
  lcd.setFreeFont(&FreeSans9pt7b);
  lcd.setTextSize(1);
  lcd.setTextDatum(TR_DATUM);
  if (displayUnit == UNIT_C) {
    lcd.drawChar('C', 0, 17, GFXFF);

  } else {
    lcd.drawChar('F', 0, 17, GFXFF);
  }

  updateDisplay(); // Update the displayed value for the new unit
}

/*************************** Sprites ***************************/

/***************************** OTA *****************************/

void displayOTASetup() {
  lcd.setTextColor(TFT_YELLOW);
  lcd.setFreeFont(&FreeSans9pt7b);
  lcd.setTextDatum(CC_DATUM);
  lcd.drawString("OTA Update", DISPLAY_W / 2, 10);

  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.setTextDatum(CC_DATUM);
  lcd.drawString("Connecting", DISPLAY_W / 2, 10);
}

void displayOTAIP(String ipaddr) {
  lcd.setViewport(0, TVAL_Y, TVAL_W, TVAL_H);
  lcd.setTextDatum(CC_DATUM);
  lcd.setTextColor(TFT_ORANGE);
  lcd.drawString(ipaddr, DISPLAY_W / 2, 25);

  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.fillScreen(IND_STATE_FILL);
  lcd.setTextColor(TFT_MAGENTA);
  lcd.setTextDatum(CC_DATUM);
  lcd.drawString("Connected", DISPLAY_W / 2, 10);
}

void displayOTAStarting(String type) {

  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.fillScreen(IND_STATE_FILL);
  lcd.setTextColor(TFT_SKYBLUE);
  lcd.setTextDatum(CC_DATUM);
  lcd.drawString("Uploading...", DISPLAY_W / 2, 10);

  lcd.setViewport(0, 116, DISPLAY_W / 2, 21);
  lcd.setTextColor(TFT_GREEN);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextDatum(CL_DATUM);
  lcd.drawString(type, 0, 10);
}

void displayOTAProgress(uint8_t progress) {

  lcd.setViewport(DISPLAY_W / 2, 116, (DISPLAY_W / 2) - 18, 21);
  lcd.setTextColor(TFT_GREEN);
  lcd.fillScreen(TFT_BLACK);
  lcd.setTextDatum(CR_DATUM);
  lcd.drawNumber(progress, 45, 10);

  lcd.setViewport((DISPLAY_W / 2) + ((DISPLAY_W / 2) - 18), 116, 18, 21);
  lcd.setTextColor(TFT_GREEN);
  lcd.fillScreen(TFT_BLACK);
  lcd.drawString("%", 18, 10);
}

void displayOTADone() {
  lcd.setViewport(0, 90, DISPLAY_W, 24);
  lcd.fillScreen(IND_STATE_FILL);
  lcd.setTextColor(TFT_GREEN);
  lcd.setTextDatum(CC_DATUM);
  lcd.drawString("Success!", DISPLAY_W / 2, 10);
}