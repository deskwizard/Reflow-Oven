#include "mypid.h"



float setpointHigh = 40.0;
float outputValue = 0.0;
float inputValue = 0.0;

float minTemp = 0.0;
float maxTemp = 0.0;

uint16_t preheatDutyCycle = 127;
uint32_t preheatTime = (2 * 60000); // minutes * (60sec * 1000ms)
uint32_t preheatStartTime = 0;
uint32_t dwellTime = (5 * 60000);
uint32_t dwellStartTime = 0;
float preheadTempTarget = 0.0;

// Different settings depending on under/over + diff if under
float nearKp = 0.01, nearKi = 0.01, nearKd = 0.1;
float aggKp = 4, aggKi = 0.2, aggKd = 1;
// float consKp = 0.5, consKi = 0.2, consKd = 0.25;

#define CONS_THRESHOLD 10
uint8_t PID_threshold = CONS_THRESHOLD;

// Specify the links and initial tuning parameters
float Kp = 0.001, Ki = 0.01, Kd = 0.01;

float consKp = 0.5, consKi = 0.1, consKd = 0.1;

// QuickPID myPID(&_ewma, &outputValue, &setpointHigh, Kp, Ki, Kd, DIRECT);
QuickPID myPID(&inputValue, &outputValue, &setpointHigh, consKp, consKi,
               consKd,                /* OPTIONS */
               myPID.pMode::pOnError, /* pOnError, pOnMeas, pOnErrorMeas */
               myPID.dMode::dOnMeas,  /* dOnError, dOnMeas */
               myPID.iAwMode::iAwCondition, /* iAwCondition, iAwClamp, iAwOff */
               myPID.Action::direct);       /* direct, reverse */



float getMaxTemp() { return maxTemp; }
float getMinTemp() { return minTemp; }
float getOutputValue() { return outputValue; }
float getKp() { return myPID.GetKp(); }
float getKi() { return myPID.GetKi(); }
float getKd() { return myPID.GetKd(); }
float getSpan() { return MAX_DUTY_CYCLE; }

float getSetpointHigh() { return setpointHigh; }

void increaseSetpointHigh(uint8_t step) {
  if (getDisplayUnit() == UNIT_C) {
    setpointHigh = setpointHigh + step;
  } else {
    setpointHigh = setpointHigh + (step * 0.555555556f);
  }
}

void decreaseSetpointHigh(uint8_t step) {
  if (getDisplayUnit() == UNIT_C) {
    setpointHigh = setpointHigh - step;
  } else {
    setpointHigh = setpointHigh - (step * 0.555555556f);
  }
}

uint8_t getDwellTime() { return dwellTime / 60000; }
uint8_t getPreheatTime() { return dwellTime / 60000; }
float getPreheatTemp() { return preheadTempTarget; }
uint8_t getPreheatDutyCycle() { return preheatDutyCycle; }

void setDwellTime(uint8_t minutes) {
  dwellTime = (minutes * 60000);
  Serial.print("Dwell time: ");
  Serial.println(dwellTime / 60000);
}

void setPreheatTime(uint8_t minutes) {
  preheatTime = (minutes * 60000);
  Serial.print("Preheat time: ");
  Serial.println(preheatTime / 60000);
}

void setPreheatDutyCycle(uint16_t dutyCycle) {
  preheatDutyCycle = dutyCycle;
  Serial.print("Prehead Duty cycle: ");
  Serial.print(preheatDutyCycle);
  Serial.print("/");
  Serial.println(MAX_DUTY_CYCLE);
}

void setThreshold(uint8_t threshold) {
  PID_threshold = threshold;
  Serial.print("Cons Threshold: ");
  Serial.println(PID_threshold);
}

void setKp(float val) {
  Kp = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Kp changed to: ");
  Serial.println(Kp, 4);
}

void setKi(float val) {
  Ki = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Ki changed to: ");
  Serial.println(Ki, 4);
}

void setKd(float val) {
  Kd = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Kd changed to: ");
  Serial.println(Kd, 4);
}

void setConsKp(float val) {
  consKp = val;
  Serial.print("Kp cons changed to: ");
  Serial.println(consKp, 4);
}

void setConsKi(float val) {
  consKi = val;
  Serial.print("Ki cons changed to: ");
  Serial.println(consKi, 4);
}

void setConsKd(float val) {
  consKd = val;
  Serial.print("Kd cons changed to: ");
  Serial.println(consKd, 4);
}

void setNearKp(float val) {
  nearKp = val;
  Serial.print("Kp near changed to: ");
  Serial.println(nearKp, 4);
}

void setNearKi(float val) {
  nearKi = val;
  Serial.print("Ki near changed to: ");
  Serial.println(nearKi, 4);
}

void setNearKd(float val) {
  nearKd = val;
  Serial.print("Kd near changed to: ");
  Serial.println(nearKd, 4);
}

void startPreheat() {

  preheadTempTarget = setpointHigh - PREHEAT_T_OFFSET;
  Serial.print("T: ");
  Serial.println(preheadTempTarget);
  Serial.print("Target preheat: ");
  Serial.println(preheadTempTarget);

  myPID.SetMode(myPID.Control::manual);
  Serial.println("Preheat started");
  setDeviceMode(MODE_PID_PREHEAT);
  preheatStartTime = millis();
  outputValue = preheatDutyCycle;
  ledcWrite(PWMChannel, outputValue);

  // TOD: that might not be optimal, investigate later
  // Reset min/max
  maxTemp = 42.0;
  minTemp = 36.0;
}

void startPID() {
  Serial.println("PID Start");
  startPreheat();
  updateStateIndicator();
}

void stopPID() {
  Serial.println("PID Stop");
  myPID.SetMode(myPID.Control::manual);
  setDeviceMode(MODE_IDLE);
  outputValue = 0;
  // Make sure we turn stuff off
  ledcWrite(PWMChannel, outputValue);
  updateStateIndicator();
  updatePowerIndicator();
}

void initPID() {
  // Configure PWM stuff here
  ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  ledcAttachPin(PIN_PWM, PWMChannel);

  myPID.SetOutputLimits(0, 127);
  myPID.SetSampleTimeUs(10000000); // 1000000 = 1 second, max uint32_t

  // Start with these tunings
  myPID.SetTunings(consKp, consKi, consKd);
}

void handlePID() {

  uint32_t currentMillis = millis();

  if (getDeviceMode() == MODE_PID_RUNNING) {
    inputValue = getAverageTemperature();

    if (inputValue >= setpointHigh) {
      maxTemp = inputValue;
      Kp = aggKp;
      Ki = aggKi;
      Kd = aggKp;
      // myPID.SetTunings(aggKp, aggKi, aggKd);
      myPID.SetTunings(Kp, Ki, Kd);
    } else if ((setpointHigh - inputValue) < float(PID_threshold)) {
      Kp = nearKp;
      Ki = nearKi;
      Kd = nearKd;
      // myPID.SetTunings(nearKp, nearKi, nearKd);
      myPID.SetTunings(Kp, Ki, Kd);
    } else {
      Kp = consKp;
      Ki = consKi;
      Kd = consKd;
      // myPID.SetTunings(consKp, consKi, consKd);
      myPID.SetTunings(Kp, Ki, Kd);
    }

    if (inputValue < setpointHigh && maxTemp != 0.0) {
      minTemp = inputValue;
    }

    myPID.Compute();
    ledcWrite(PWMChannel, outputValue);

    static uint32_t previousMillis = 0;

    if ((uint32_t)(currentMillis - previousMillis) >= 2000) {

      Serial.print(inputValue);
      Serial.print("°C  (");
      Serial.print(CtoF(inputValue));
      Serial.print("°F)");

      Serial.print("  -  ");
      Serial.print(getOutputValue(), 0);
      Serial.print("/");
      Serial.print(getSpan(), 0);
      Serial.print(" (");
      Serial.print((getOutputValue() / getSpan()) * 100, 0);
      Serial.print("%)");
      // Serial.println();

      Serial.print("   Kp: ");
      Serial.print(myPID.GetKp());
      Serial.print("  Ki: ");
      Serial.print(myPID.GetKi());
      Serial.print("  Kd: ");
      Serial.println(myPID.GetKd());
      Serial.println();

      previousMillis = currentMillis;
    }
  } // running

  else if (getDeviceMode() == MODE_PID_PREHEAT) {

    // if (((uint32_t)(currentMillis - preheatStartTime) >= preheatTime) &&
    //     preheatStartTime != 0) {
    if (getAverageTemperature() >= preheadTempTarget) {
      Serial.println("Preheat end");
      setDeviceMode(MODE_PID_DWELL);
      updateStateIndicator();
      updatePIDValues();
      outputValue = 0;
      ledcWrite(PWMChannel, outputValue);
      preheatStartTime = 0;
      dwellStartTime = millis();
    }

    // if (((uint32_t)(currentMillis - preheatStartTime) >= preheatTime) &&
    //     preheatStartTime != 0) {
    //   Serial.println("Preheat end");
    //   setDeviceMode(MODE_PID_DWELL);
    //   updateStateIndicator();
    //   updatePIDValues();
    //   outputValue = 0;
    //   ledcWrite(PWMChannel, outputValue);
    //   preheatStartTime = 0;
    //   dwellStartTime = millis();
    // }
  } // preheat

  else if (getDeviceMode() == MODE_PID_DWELL) {
    if (((uint32_t)(currentMillis - dwellStartTime) >= dwellTime) &&
        dwellStartTime != 0) {
      Serial.println("Dwell end");
      setDeviceMode(MODE_PID_RUNNING);
      updateStateIndicator();
      myPID.SetMode(myPID.Control::automatic);
      dwellStartTime = 0;
    } // dwell
  }
}