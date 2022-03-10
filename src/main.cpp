#include "control.h"
#include "defines.h"
#include "display.h"
#include "keypad.h"
#include "mypid.h"
#include "sensor.h"

#ifdef OTA_ENABLED
#include "credentials.h"
#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#endif
void initOTA();
void handleOTA();

#ifdef BT_SERIAL_ENABLED
BluetoothSerial SerialBT;
#endif

void printSerialHelp();

float CtoF(float celsius) { return (celsius * 1.8f) + 32.0f; }

void setup() {

  Serial.begin(115200);
  delay(2000);
  Serial.println("Hello");
  Serial.println();
#ifdef BT_SERIAL_ENABLED
  SerialBT.begin("Reflow Oven"); // Bluetooth device name
  SerialBT.println("Hello");
  SerialBT.println();
#endif
  initOTA();
  initDisplay();
  initOutputs();
  initSensor();
  initKeypad();
  initPID();
}

void loop() {
  handleDisplay();
  handleKeypad();
  handleSensor();
  handlePID();
  handleOTA();

  if (Serial.available()) {

    unsigned char c = Serial.read();
    float readValueF;
    uint16_t readValueC;

    switch (c) {

    case 'p':
      readValueF = Serial.parseFloat();
      setKp(readValueF);
      break;
    case 'i':
      readValueF = Serial.parseFloat();
      setKi(readValueF);
      break;
    case 'd':
      readValueF = Serial.parseFloat();
      setKd(readValueF);
      break;

    case 'P':
      Serial.print("Kp: ");
      Serial.println(getKp(), 4);
      break;
    case 'I':
      Serial.print("Ki: ");
      Serial.println(getKi(), 4);
      break;
    case 'D':
      Serial.print("Kd: ");
      Serial.println(getKd(), 4);
      break;

    case 'q':
      readValueC = Serial.parseInt();
      setDwellTime(readValueC);
      break;

    case 'c':
      readValueC = Serial.parseInt();
      setPreheatDutyCycle(readValueC);
      break;
    case 'v':
      readValueC = Serial.parseInt();
      setPreheatTime(readValueC);
      break;
    case 'w':
      readValueC = Serial.parseInt();
      setDwellTime(readValueC);
      break;
    case 'h':
      printSerialHelp();
      break;
    }
  }
}

void printSerialHelp() {
  Serial.println("  Command            Action");
  Serial.println("---------------------------------------------");
  Serial.println("  w(int)             Set dwell time");
  Serial.println("  v(int)             Set preheat time");
  Serial.println("  c(int)             Set preheat duty cycle");
  Serial.println();
  Serial.println("  P                  Print Kp value");
  Serial.println("  I                  Print Ki value");
  Serial.println("  D                  Print Kd value");
  Serial.println();
  Serial.println("  p(float)           Set Kp");
  Serial.println("  i(float)           Set Ki");
  Serial.println("  d(float)           Set Kd");
  Serial.println("---------------------------------------------");
  Serial.println();
}

void initOTA() {
#ifdef OTA_ENABLED
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // Port defaults to 3232
  // ArduinoOTA.setPort(3232);

  // Hostname defaults to esp3232-[MAC]
  // ArduinoOTA.setHostname("myesp32");

  // No authentication by default
  // ArduinoOTA.setPassword("admin");

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
        // using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() { Serial.println("\nEnd"); })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  Serial.println("Ready");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
#endif
}

void handleOTA() {
#ifdef OTA_ENABLED
  ArduinoOTA.handle();
#endif
}