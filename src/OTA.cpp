#include "defines.h"
#ifdef OTA_ENABLED
#include <esp_task_wdt.h>
#include "credentials.h"

#include "display.h"
#include "defines.h"

#include <ArduinoOTA.h>
#include <ESPmDNS.h>
#include <WiFi.h>
#include <WiFiUdp.h>

String getSSID() {
  return WiFi.SSID();
}

void handleOTA() { ArduinoOTA.handle(); }

void initOTA() {

  displayOTASetup();

  // Start wifi for OTA
  Serial.println("Starting Wifi...");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);
  while (WiFi.waitForConnectResult() != WL_CONNECTED) {
    //displayOTAFail();
    ;
    Serial.println("Connection Failed! Rebooting...");
    delay(5000);
    ESP.restart();
  }

  // ArduinoOTA.setPort(3232);              // Port defaults to 3232
  ArduinoOTA.setHostname("ReflowOven_OTA"); // Hostname defaults to esp3232-[MAC]

  // ArduinoOTA.setPassword("admin");       // No authentication by default

  // Password can be set with it's md5 value as well
  // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
  // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "Flash";
        else // U_SPIFFS
          type = "Spiffs";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS
        // using SPIFFS.end()
        Serial.println("Start updating " + type);

        displayOTAStarting(type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
        displayOTADone();
        delay(2000);
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        esp_task_wdt_reset();

        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        Serial.print("Progress: ");

        uint8_t actual_progress = progress / (total / 100);
        Serial.println(actual_progress);

        // show progress on lcd
        displayOTAProgress(actual_progress);
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

  //displayOTASuccess(WiFi.localIP().toString());

  Serial.print("OTA Ready at IP address: ");
  
  Serial.println(WiFi.localIP());
  displayOTAIP(WiFi.localIP().toString());

  delay(5000);

  //  while(1){}
}
#else

void initOTA() { return; }

void handleOTA() { return; }

#endif
