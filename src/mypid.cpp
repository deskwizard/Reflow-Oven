#include "mypid.h"

extern float _ewma;
float setpointHigh = 40.0;

float outputValue = 0.0;
// const float outputSpan = MAX_DUTY_CYCLE;
uint16_t preheatDutyCycle = 127;
uint32_t preheatTime = (2 * 60000); // minutes * (60sec * 1000ms)
uint32_t preheatStartTime = 0;
uint32_t dwellTime = (3 * 60000);
uint32_t dwellStartTime = 0;

// Different settings depending on under/over + diff if under
float nearKp = 0.001, nearKi = 0.01, nearKd = 0.01;
float aggKp = 4, aggKi = 0.2, aggKd = 1;
float consKp = 0.5, consKi = 0.2, consKd = 0.25;

// Specify the links and initial tuning parameters
float Kp = 0.001, Ki = 0.01, Kd = 0.01;

// QuickPID myPID(&_ewma, &outputValue, &setpointHigh, Kp, Ki, Kd, DIRECT);
QuickPID myPID(&_ewma, &outputValue, &setpointHigh, consKp, consKi,
               consKd,                /* OPTIONS */
               myPID.pMode::pOnError, /* pOnError, pOnMeas, pOnErrorMeas */
               myPID.dMode::dOnMeas,  /* dOnError, dOnMeas */
               myPID.iAwMode::iAwCondition, /* iAwCondition, iAwClamp, iAwOff */
               myPID.Action::direct);       /* direct, reverse */

uint8_t deviceMode = MODE_IDLE;

float getOutputValue() { return outputValue; }
float getKp() { return myPID.GetKp(); }
float getKi() { return myPID.GetKi(); }
float getKd() { return myPID.GetKd(); }
float getSpan() { return MAX_DUTY_CYCLE; }

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

// These don't work

void setKp(float val) {
  myPID.SetTunings(val, Ki, Kd);
  Serial.print("Kp changed to: ");
  Serial.println(Kp, 4);
}

void setKi(float val) {
  myPID.SetTunings(Kp, val, Kd);
  Serial.print("Ki changed to: ");
  Serial.println(Ki, 4);
}

void setKd(float val) {
  myPID.SetTunings(Kp, Ki, val);
  Serial.print("Kd changed to: ");
  Serial.println(Kd, 4);
}

void startPreheat() {
  myPID.SetMode(myPID.Control::manual);
  Serial.println("Preheat started");
  deviceMode = MODE_PID_PREHEAT;
  preheatStartTime = millis();
  outputValue = preheatDutyCycle;
  ledcWrite(PWMChannel, outputValue);
}

void startPID() {
  Serial.println("PID Start");
  startPreheat();
  updateStateIndicator();
  // myPID.SetMode(myPID.Control::automatic);
}

void stopPID() {
  Serial.println("PID Stop");
  myPID.SetMode(myPID.Control::manual);
  deviceMode = MODE_IDLE;
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

  //   myPID.SetOutputLimits(0, 255);
  //   myPID.SetSampleTimeUs(100000);

  // Start with these tunings
  myPID.SetTunings(consKp, consKi, consKd);
}

void handlePID() {

  uint32_t currentMillis = millis();

  if (deviceMode == MODE_PID_RUNNING) {

    if (_ewma > setpointHigh) {
      myPID.SetTunings(aggKp, aggKi, aggKd);
    } else if ((setpointHigh - _ewma) > 10.0) {
      myPID.SetTunings(nearKp, nearKi, nearKd);
    } else {
      myPID.SetTunings(consKp, consKi, consKd);
    }

    myPID.Compute();
    ledcWrite(PWMChannel, outputValue);

    static uint32_t previousMillis = 0;

    if ((uint32_t)(currentMillis - previousMillis) >= 2000) {

      Serial.print(_ewma);
      Serial.print("°C  (");
      Serial.print(CtoF(_ewma));
      Serial.print("°F)");

      Serial.print("  -  ");
      Serial.print(getOutputValue(), 0);
      Serial.print("/");
      Serial.print(getSpan(), 0);
      Serial.print(" (");
      Serial.print((getOutputValue() / getSpan()) * 100, 0);
      Serial.print("%)");
      Serial.println();

      //   Serial.print("Kp: ");
      //   Serial.print(myPID.GetKp());
      //   Serial.print("  Ki: ");
      //   Serial.print(myPID.GetKi());
      //   Serial.print("  Kd: ");
      //   Serial.println(myPID.GetKd());
      //   Serial.println();

      previousMillis = currentMillis;
    }
  } // running

  // uint32_t currentMillis = millis();

  else if (deviceMode == MODE_PID_PREHEAT) {

    if (((uint32_t)(currentMillis - preheatStartTime) >= preheatTime) &&
        preheatStartTime != 0) {
      Serial.println("Preheat end");
      deviceMode = MODE_PID_DWELL;
      updateStateIndicator();
      outputValue = 0;
      ledcWrite(PWMChannel, outputValue);
      preheatStartTime = 0;
      dwellStartTime = millis();
    }
  } // preheat

  else if (deviceMode == MODE_PID_DWELL) {
    if (((uint32_t)(currentMillis - dwellStartTime) >= dwellTime) &&
        dwellStartTime != 0) {
      Serial.println("Dwell end");
      deviceMode = MODE_PID_RUNNING;
      updateStateIndicator();
      myPID.SetMode(myPID.Control::automatic);
      dwellStartTime = 0;
    } // dwell
  }
}