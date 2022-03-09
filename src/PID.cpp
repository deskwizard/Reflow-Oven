#include "control.h"
#include "display.h"

extern double _ewma;
/***************************************************************************
  sTune QuickPID Example (MAX6675, PTC Heater / SSR / Software PWM)
  This sketch does on-the-fly tuning and PID control. Tuning parameters are
  quickly determined and applied during temperature ramp-up to setpointHigh.
  View results using serial plotter.
  Reference: https://github.com/Dlloydev/sTune/wiki/Examples_MAX6675_PTC_SSR
  ***************************************************************************/
#include <QuickPID.h>
#include <sTune.h>

bool tuning = false;

extern float setpointHigh;

float outputValue = 0.0;
const float outputSpan = 255.0;

// user settings
uint32_t settleTimeSec = 10;
uint32_t testTimeSec = 500; // runPid interval = testTimeSec / samples
const uint16_t samples = 500;
const float inputSpan = 200.0;

float outputStart = 0.0;
float outputStep = 127.0; // Portion of output span
float tempLimit = 80.0;
uint8_t debounce = 1;

// variables
float Input;
float Kp, Ki, Kd;

sTune tuner =
    sTune(&Input, &outputValue, tuner.ZN_PID, tuner.directIP, tuner.printOFF);
QuickPID myPID(&Input, &outputValue, &setpointHigh);

float getOutputValue() { return outputValue; }
float getKp() { return Kp; }
float getKi() { return Ki; }
float getKd() { return Kd; }

void initPIDsTune() {

  outputValue = 0;
  tuner.Configure(inputSpan, outputSpan, outputStart, outputStep, testTimeSec,
                  settleTimeSec, samples);
  tuner.SetEmergencyStop(tempLimit);
}

void toggleTuning() {

  tuning = !tuning;
  Serial.print("tuning: ");
  Serial.print(tuning);
#ifdef BT_SERIAL_ENABLED
  SerialBT.print("tuning: ");
  SerialBT.print(tuning);
#endif

  outputValue = 0;

  if (tuning) {
    initPIDsTune();
  } else {
    digitalWrite(PIN_PWM, LOW);
  }
  updateStateIndicator();
}

void handlePIDsTune() {
  // If we're not tuning, don't run it.
  if (!tuning) {
    return;
  }

  float optimumOutput = tuner.softPwm(PIN_PWM, Input, outputValue, setpointHigh,
                                      outputSpan, debounce);

  switch (tuner.Run()) {
  case tuner.sample: // active once per sample during test
    Input = _ewma;
    // outputValue scale 0.5, plot every 3rd sample <== why??
    // tuner.plotter(Input, outputValue, setpointHigh, 0.5f, 3);

    printValues();
    printValuesBT();
    break;

  case tuner.tunings:                    // active just once when sTune is done
    tuner.GetAutoTunings(&Kp, &Ki, &Kd); // sketch variables updated by sTune
    myPID.SetOutputLimits(0, outputSpan * 0.1);
    myPID.SetSampleTimeUs((outputSpan - 1) * 1000);
    debounce = 0; // ssr mode
    outputValue = outputStep;
    myPID.SetMode(myPID.Control::automatic); // the PID is turned on
    myPID.SetProportionalMode(myPID.pMode::pOnMeas);
    myPID.SetAntiWindupMode(myPID.iAwMode::iAwClamp);
    myPID.SetTunings(Kp, Ki, Kd); // update PID with the new tunings
    break;

  case tuner.runPid: // active once per sample after tunings
    Input = _ewma;
    myPID.Compute();
    tuner.plotter(Input, optimumOutput, setpointHigh, 0.5f, 3);
    break;
  }
}

void printValues() {
  Serial.print("Temp: ");
  Serial.print(_ewma);
  Serial.print("°C  (");
  Serial.print(CtoF(_ewma));
  Serial.print("°F)");

  Serial.print("  SP: ");
  Serial.print(setpointHigh);
  Serial.print("°C");

  Serial.print("  OutVal: ");
  Serial.print(outputValue, 0);
  Serial.print("/255 (");
  Serial.print((outputValue / outputSpan) * 100.0);
  Serial.print("%)  ");
  // Serial.print("Kp: ");
  // Serial.print(Kp, 4);
  // Serial.print("  Ki: ");
  // Serial.print(Ki, 4);
  // Serial.print("  Kd: ");
  // Serial.print(Kd, 4);
  Serial.println();
}

void printValuesBT() {
#ifdef BT_SERIAL_ENABLED
  SerialBT.print("Temp: ");
  SerialBT.print(_ewma);
  SerialBT.print("°C  (");
  SerialBT.print(CtoF(_ewma));
  SerialBT.print("°F)");
  SerialBT.print("  SP: ");
  SerialBT.print(setpointHigh);
  SerialBT.print("°C");
  SerialBT.print("  OutVal: ");
  SerialBT.print(outputValue, 0);
  SerialBT.print("/255 (");
  SerialBT.print((outputValue / outputSpan) * 100.0);
  SerialBT.print("%)  ");
  // SerialBT.print("Kp: ");
  // SerialBT.print(Kp, 4);
  // SerialBT.print("  Ki: ");
  // SerialBT.print(Ki, 4);
  // SerialBT.print("  Kd: ");
  // SerialBT.print(Kd, 4);
  SerialBT.println();
#endif
}