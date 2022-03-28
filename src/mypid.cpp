#include "mypid.h"

/****************************************************************/
// uint32_t currentMillis = millis();
// uint32_t seconds = currentMillis / 1000;
// uint32_t minutes = seconds / 60;
// uint32_t hours = minutes / 60;
// uint32_t days = hours / 24;
// currentMillis %= 1000;
// seconds %= 60;
// minutes %= 60;
// hours %= 24;
/****************************************************************/
float setpointHigh = 60.0;
float outputValue = 0.0;
float inputValue = 0.0;

float minTemp = 0.0;
float maxTemp = 0.0;

uint16_t preheatDutyCycle = 127;
uint32_t preheatTime = (2 * 60000); // minutes * (60sec * 1000ms)
uint32_t preheatStartTime = 0;
uint32_t dwellTime = (2 * 60000);
uint32_t dwellStartTime = 0;
float preheatTempTarget = 0.0;
uint8_t preheatPercent = 66;

uint32_t runningStartTime = 0;

// Different settings depending on under/over + diff if under
float nearKp = 0.25, nearKi = 0.01, nearKd = 0.1;
float aggKp = 4, aggKi = 0.2, aggKd = 1;

float Kp = 2.0, Ki = 0.2, Kd = 2.0;

#define CONS_THRESHOLD 10
uint8_t PID_threshold = CONS_THRESHOLD;

// Specify the links and initial tuning parameters
float consKp = 0.01, consKi = 0.01, consKd = 0.5;

QuickPID myPID(&inputValue, &outputValue, &setpointHigh, Kp, Ki, Kd, myPID.Action::direct);
// QuickPID myPID(&inputValue, &outputValue, &setpointHigh, consKp, consKi,
//                consKd,                /* OPTIONS */
//                myPID.pMode::pOnError, /* pOnError, pOnMeas, pOnErrorMeas */
//                myPID.dMode::dOnMeas,  /* dOnError, dOnMeas */
//                myPID.iAwMode::iAwCondition, /* iAwCondition, iAwClamp, iAwOff */
//                myPID.Action::direct);       /* direct, reverse */

void startPreheat()
{

  preheatTempTarget = setpointHigh * (float(preheatPercent) / 100.0);

  // preheatTempTarget = (setpointHigh - getAverageTemperature()) * (float(preheatPercent) / 100.0)

  Serial.print("T: ");
  Serial.println(preheatTempTarget);
  Serial.print("Target preheat: ");
  Serial.println(preheatTempTarget);

  myPID.SetMode(myPID.Control::manual);
  Serial.println("Preheat started");
  setDeviceMode(MODE_PID_PREHEAT);
  preheatStartTime = millis();
  outputValue = preheatDutyCycle;
  ledcWrite(PWMChannel, outputValue);

  // TODO: that might not be optimal, investigate later
  // Reset min/max
  maxTemp = 0.0;
  // minTemp = 0.0;
  minTemp = getAverageTemperature();
}

void startPID()
{
  Serial.println("PID Start");
  setHeatersState(ON);
  startPreheat();
  updateStateIndicator();
}

void stopPID()
{
  Serial.println("PID Stop");
  myPID.SetMode(myPID.Control::manual);
  setDeviceMode(MODE_IDLE);
  outputValue = 0.0;

  // Make sure we turn stuff off
  setHeatersState(OFF);
  ledcWrite(PWMChannel, outputValue);

  updateStateIndicator();
  updatePowerIndicator();
}

void initPID()
{
  // Configure PWM stuff here
  ledcSetup(PWMChannel, PWMFreq, PWMResolution);
  /* Attach the LED PWM Channel to the GPIO Pin */
  ledcAttachPin(PIN_PWM, PWMChannel);

  myPID.SetOutputLimits(0, 127);
  myPID.SetSampleTimeUs(1000000); // 1000000 = 1 second, max uint32_t

  // Start with these tunings
  // myPID.SetTunings(consKp, consKi, consKd);
}

void time()
{
  static uint8_t savedSeconds = 0;

   uint32_t timeDelta = millis() - runningStartTime;
   
   uint8_t seconds = (timeDelta / 1000) % 60;
   uint8_t minutes = (timeDelta / 60000) % 60;

   uint32_t hours = minutes / 60; // Will be wrong atm

  // uint32_t currentMillis = millis();
  // uint32_t timeDelta = millis() - runningStartTime;
  // uint8_t seconds = (timeDelta / 1000) % 60;
  // uint8_t minutes = seconds % 60;
  // uint8_t hours = minutes % 24;
  // uint32_t days = hours / 24;
  // currentMillis %= 1000;
  //seconds %= 60;
  //minutes %= 60;
  //hours %= 24;

  if (seconds != savedSeconds)
  {
    savedSeconds = seconds;

    if (hours < 10)
    {
      Serial.write('0');
    }
    Serial.print(hours);
    Serial.write(':');

    if (minutes < 10)
    {
      Serial.write('0');
    }
    Serial.print(minutes);
    Serial.write(':');

    if (seconds < 10)
    {
      Serial.write('0');
    }
    Serial.print(seconds);
    Serial.println();
  }
}

void handlePID()
{

  time();

  uint32_t currentMillis = millis();

  // uint32_t currentMillis = millis();
  //  uint32_t timeDelta = currentMillis - runningStartTime;
  //  uint32_t seconds = timeDelta / 1000;
  //  uint32_t minutes = seconds / 60;
  //  uint32_t hours = minutes / 60;
  //  uint32_t days = hours / 24;
  //  currentMillis %= 1000;
  //  seconds %= 60;
  //  minutes %= 60;
  //  hours %= 24;

  inputValue = getAverageTemperature();

  if (getDeviceMode() == MODE_PID_RUNNING)
  {

    //   Kp = aggKp;
    //   Ki = aggKi;
    //   Kd = aggKp;
    //   // myPID.SetTunings(aggKp, aggKi, aggKd);
    //   myPID.SetTunings(Kp, Ki, Kd);
    // }
    // // else if ((setpointHigh - inputValue) < float(PID_threshold)) {
    // //   Kp = nearKp;
    // //   Ki = nearKi;
    // //   Kd = nearKd;
    // //   // myPID.SetTunings(nearKp, nearKi, nearKd);
    // //   myPID.SetTunings(Kp, Ki, Kd);
    // // } else {
    // //   Kp = consKp;
    // //   Ki = consKi;
    // //   Kd = consKd;
    // //   // myPID.SetTunings(consKp, consKi, consKd);
    // //   myPID.SetTunings(Kp, Ki, Kd);
    // // }
    // else {
    //   Kp = nearKp;
    //   Ki = nearKi;
    //   Kd = nearKd;
    //   // myPID.SetTunings(nearKp, nearKi, nearKd);
    //   myPID.SetTunings(Kp, Ki, Kd);
    // }

    // inputValue = getAverageTemperature();

    if (inputValue > maxTemp)
    {
      maxTemp = inputValue;
    }

    // if (minTemp != 0.0 && inputValue < minTemp) {
    //   minTemp = inputValue;
    // }

    // if (inputValue < minTemp && inputValue < setpointHigh && maxTemp != 0.0) {
    //   minTemp = inputValue;
    // }

    myPID.Compute();
    ledcWrite(PWMChannel, outputValue);

    // time();

    static uint32_t previousMillis = 0;

    if ((uint32_t)(currentMillis - previousMillis) >= 5000)
    {

      // uint32_t timeDelta = currentMillis - runningStartTime;
      // uint32_t seconds = timeDelta / 1000;
      // uint32_t minutes = seconds / 60;
      // uint32_t hours = minutes / 60;
      // // uint32_t days = hours / 24;
      // // currentMillis %= 1000;
      // seconds %= 60;
      // minutes %= 60;
      // hours %= 24;
      // Serial.printf("%i:%i:%i\n", hours, minutes, seconds);

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

      // Serial.print("   Kp: ");
      // Serial.print(myPID.GetKp());
      // Serial.print("  Ki: ");
      // Serial.print(myPID.GetKi());
      // Serial.print("  Kd: ");
      // Serial.println(myPID.GetKd());
      Serial.println();

      previousMillis = currentMillis;
    }
  } // running

  else if (getDeviceMode() == MODE_PID_PREHEAT)
  {

    if (inputValue > maxTemp)
    {
      maxTemp = inputValue;
    }
    // if (((uint32_t)(currentMillis - preheatStartTime) >= preheatTime) &&
    //     preheatStartTime != 0) {
    if (getAverageTemperature() >= preheatTempTarget)
    {
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

  else if (getDeviceMode() == MODE_PID_DWELL)
  {
    if ((((uint32_t)(currentMillis - dwellStartTime) >= dwellTime) &&
         dwellStartTime != 0) ||
        inputValue < inputValue)
    {
      Serial.println("Dwell end");
      setDeviceMode(MODE_PID_RUNNING);
      updateStateIndicator();
      myPID.SetMode(myPID.Control::automatic);
      dwellStartTime = 0;
      runningStartTime = millis();
    } // dwell
  }
}

/**********************************************************************************/

float getMaxTemp() { return maxTemp; }
float getMinTemp() { return minTemp; }
float getOutputValue() { return outputValue; }
float getKp() { return myPID.GetKp(); }
float getKi() { return myPID.GetKi(); }
float getKd() { return myPID.GetKd(); }
float getSpan() { return MAX_DUTY_CYCLE; }

float getSetpointHigh() { return setpointHigh; }

void increaseSetpointHigh(uint8_t step)
{
  if (getDisplayUnit() == UNIT_C)
  {
    setpointHigh = setpointHigh + step;
  }
  else
  {
    setpointHigh = setpointHigh + (step * 0.555555556f);
  }
}

void decreaseSetpointHigh(uint8_t step)
{
  if (getDisplayUnit() == UNIT_C)
  {
    setpointHigh = setpointHigh - step;
  }
  else
  {
    setpointHigh = setpointHigh - (step * 0.555555556f);
  }
}

uint8_t getDwellTime() { return dwellTime / 60000; }
uint8_t getPreheatTime() { return dwellTime / 60000; }
float getPreheatTemp() { return preheatTempTarget; }
uint8_t getPreheatDutyCycle() { return preheatDutyCycle; }
uint8_t getPreheatPercent() { return preheatPercent; }
void setPreheatPercent(uint8_t percent) { preheatPercent = percent; }

void setDwellTime(uint8_t minutes)
{
  dwellTime = (minutes * 60000);
  Serial.print("Dwell time: ");
  Serial.println(dwellTime / 60000);
}

void setPreheatTime(uint8_t minutes)
{
  preheatTime = (minutes * 60000);
  Serial.print("Preheat time: ");
  Serial.println(preheatTime / 60000);
}

void setPreheatDutyCycle(uint16_t dutyCycle)
{
  preheatDutyCycle = dutyCycle;
  Serial.print("Prehead Duty cycle: ");
  Serial.print(preheatDutyCycle);
  Serial.print("/");
  Serial.println(MAX_DUTY_CYCLE);
}

void setThreshold(uint8_t threshold)
{
  PID_threshold = threshold;
  Serial.print("Cons Threshold: ");
  Serial.println(PID_threshold);
}

void setKp(float val)
{
  Kp = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Kp changed to: ");
  Serial.println(Kp, 4);
}

void setKi(float val)
{
  Ki = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Ki changed to: ");
  Serial.println(Ki, 4);
}

void setKd(float val)
{
  Kd = val;
  myPID.SetTunings(Kp, Ki, Kd);
  Serial.print("Kd changed to: ");
  Serial.println(Kd, 4);
}

void setConsKp(float val)
{
  consKp = val;
  Serial.print("Kp cons changed to: ");
  Serial.println(consKp, 4);
}

void setConsKi(float val)
{
  consKi = val;
  Serial.print("Ki cons changed to: ");
  Serial.println(consKi, 4);
}

void setConsKd(float val)
{
  consKd = val;
  Serial.print("Kd cons changed to: ");
  Serial.println(consKd, 4);
}

void setNearKp(float val)
{
  nearKp = val;
  Serial.print("Kp near changed to: ");
  Serial.println(nearKp, 4);
}

void setNearKi(float val)
{
  nearKi = val;
  Serial.print("Ki near changed to: ");
  Serial.println(nearKi, 4);
}

void setNearKd(float val)
{
  nearKd = val;
  Serial.print("Kd near changed to: ");
  Serial.println(nearKd, 4);
}