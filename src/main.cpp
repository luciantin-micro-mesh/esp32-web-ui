#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"


#ifndef SSID
#define SSID                  "a"
#endif

#ifndef SSID_PASS
#define SSID_PASS             "12345678"
#endif

#ifdef ESP32_CAM
#define LED_BUILTIN           33
#define FLASH_PIN             4
#endif

#ifndef LED_BUILTIN
#define LED_BUILTIN           4
#endif

#ifndef FLASH_PIN
#define FLASH_PIN             4
#endif

#define CORE_1 0
#define CORE_2 1


const char* WIFI_SSID = "a";
const char* WIFI_SSID_PASS = "12345678";

// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    while(1) digitalWrite(LED_BUILTIN, millis() % 200 < 50 ? HIGH : LOW);
  }
}

////////////////////////////////////////////////////////////////////////////////

void initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_SSID_PASS);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(500);
  }
  Serial.printf(" %s\n", WiFi.localIP().toString().c_str());
}


void TaskSerialPrint(void *pvParameters){
  (void) pvParameters;

  for(;;){
    Serial.println("Hello");
    vTaskDelay(100);
  }
}

void TaskBlink(void *pvParameters){
  (void) pvParameters;

  for(;;){
    digitalWrite(LED_BUILTIN, HIGH);
    vTaskDelay(100);
    digitalWrite(LED_BUILTIN, LOW);
    vTaskDelay(100);
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);


  Serial.begin(115200);
  delay(10);
  Serial.println("Init .....");

  xTaskCreatePinnedToCore(TaskSerialPrint, "TaskPrint", 1024, NULL, 2, NULL, CORE_1);
  xTaskCreatePinnedToCore(TaskBlink, "TaskBlink", 1024, NULL, 2, NULL, CORE_2);


}

void loop() {

}