#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "wifi_settings.h"
// Network Settings
// Build flags || wifi_settings.h
// Wifi passwd and SSID :
// 1. add wifi_settings.h file inside the include dir or uncomment the defines and comment the include
// 2. add wifi passwd &  SSID
//#define NET_SSID "****"
//#define NET_PWD "****"

// #ifndef SSID
// #define SSID                  NET_SSID
// #endif

// #ifndef SSID_PASS
// #define SSID_PASS             NET_PWD
// #endif

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

#define DEBUG true
#define DebugPrint(a) if(DEBUG) Serial.println(a);

// const char* WIFI_SSID = "a";
// const char* WIFI_SSID_PASS = "12345678";

////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// SPIFFS initialization
// ----------------------------------------------------------------------------

void initSPIFFS() {
  if (!SPIFFS.begin()) {
    Serial.println("Cannot mount SPIFFS volume...");
    while(1) digitalWrite(LED_BUILTIN, millis() % 200 < 50 ? HIGH : LOW);
  }
  else Serial.println("SPIFFS init OK");
}

////////////////////////////////////////////////////////////////////////////////
// ----------------------------------------------------------------------------
// WebServer initialization
// ----------------------------------------------------------------------------

AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

void network_scan(){
  Serial.println("scan start");

  int n = WiFi.scanNetworks();
  Serial.println("scan done");
  if (n == 0) {
      Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.print(WiFi.SSID(i));
      Serial.print(" (");
      Serial.print(WiFi.RSSI(i));
      Serial.print(")");
      Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
      delay(10);
    }
  }
}


void notifyWs(void *pvParameters){
  vTaskDelay(1000);
  (void) pvParameters;

  for(;;){
    ws.textAll("Hello World!");
    vTaskDelay(100);
  }
}

void onWsEvent(AsyncWebSocket       *server,  //
             AsyncWebSocketClient *client,  //
             AwsEventType          type,    // the signature of this function is defined
             void                 *arg,     // by the `AwsEventHandler` interface
             uint8_t              *data,    //
             size_t                len) {   //

    switch (type) {
        case WS_EVT_CONNECT:
            Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
            break;
        case WS_EVT_DISCONNECT:
            Serial.printf("WebSocket client #%u disconnected\n", client->id());
            break;
        case WS_EVT_DATA:
        case WS_EVT_PONG:
        case WS_EVT_ERROR:
            break;
    }
}

void initWebSocket() {
    ws.onEvent(onWsEvent);
    server.addHandler(&ws);
}

void initWiFi() {
  // WiFi.begin(WIFI_SSID, WIFI_SSID_PASS);
  WiFi.begin(NET_SSID, NET_PWD);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  Serial.printf("\nLocal IP : %s\n", WiFi.localIP().toString().c_str());
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

// ArUploadHandlerFunction handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
//   DebugPrint(1234);

//   if(!index){
//     Serial.printf("UploadStart: %s\n", filename.c_str());
//   }
//   for(size_t i=0; i<len; i++){
//     Serial.write(data[i]);
//   }
//   if(final){
//     Serial.printf("UploadEnd: %s, %u B\n", filename.c_str(), index+len);
//   }
//   request->send(200);
// }


void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.println((String)"UploadStart: " + filename);
    // open the file on first call and store the file handle in the request object
    request->_tempFile = SPIFFS.open("/"+filename, "w");
  }
  if(len) {
    for(size_t i=0; i<len; i++){
      Serial.write(data[i]);
    }
    // stream the incoming chunk to the opened file
    request->_tempFile.write(data,len);
  }
  if(final){
    Serial.println((String)"UploadEnd: " + filename + "," + index+len);
    // close the file handle as the upload is now done
    request->_tempFile.close();
    request->send(200, "text/plain", "File Uploaded !");

    File root = SPIFFS.open("/");
    File file = root.openNextFile();

    while(file){

    Serial.print("FILE: ");
    Serial.println(file.name());

    file = root.openNextFile();
    }
  }
}

void handleUploadToSerial(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.println((String)"UploadStart: " + filename);
  }
  for(size_t i=0; i<len; i++){
    Serial.write(data[i]);
  }
  if(final){
    Serial.println((String)"UploadEnd: " + filename + "," + index+len);
    request->redirect("/");
  }

}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);


  Serial.begin(115200);
  delay(10);
  Serial.println("Init .....");
  WiFi.mode(WIFI_STA);

  // network_scan();

  initWiFi();
  initWebSocket();
  initSPIFFS();


  // xTaskCreatePinnedToCore(TaskSerialPrint, "TaskPrint", 1024, NULL, 2, NULL, CORE_1);
  xTaskCreatePinnedToCore(notifyWs, "notifyWs", 1024, NULL, 2, NULL, CORE_2);

  server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request) {},
      [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
                    size_t len, bool final) {handleUpload(request, filename, index, data, len, final);}
  );

  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    DebugPrint(0);
    request->send(SPIFFS, "/index.html", String(), false, NULL);
  });

  server.on("/main.js", HTTP_GET, [](AsyncWebServerRequest *request){
    DebugPrint(1);
    request->send(SPIFFS, "/main.js", String(), false, NULL);
  });

  server.on("/style.css", HTTP_GET, [](AsyncWebServerRequest *request){
    DebugPrint(2);
    request->send(SPIFFS, "/style.css", String(), false, NULL);
  });

  server.on("/favicon.png", HTTP_GET, [](AsyncWebServerRequest *request){
    DebugPrint(3);
    request->send(SPIFFS, "/favicon.png", String(), false, NULL);
  });

  server.on("/files", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send(SPIFFS, "/files.html", "text/html");
  });

  server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");

  // server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
  //   DebugPrint(4);
  // },
  //   [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
  //                 size_t len, bool final) {handleUpload(request, filename, index, data, len, final);}
  // );

  // server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request) {
  //   request->send(200);
  //   DebugPrint(4);
  //     }, handleUpload);

  // server.on("/upload", HTTP_POST, [](AsyncWebServerRequest *request) {
  //   DebugPrint(4);
  //   request->send(200);
  // }, handleUpload);

  // server.on("^(.*?)$", HTTP_GET, [](AsyncWebServerRequest *request){
  //   DebugPrint(5);
  //   Serial.println("Web Connection");
  //   request->send(200, "text/plain", "Hello World");
  // });



  // server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request) {
  //       DebugPrint(4);
  // }, handleUpload);

  // server.onFileUpload([](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final){
  //   if(!index)
  //     Serial.printf("UploadStart: %s\n", filename.c_str());
  //   Serial.printf("%s", (const char*)data);
  //   if(final)
  //     Serial.printf("UploadEnd: %s (%u)\n", filename.c_str(), index+len);
  // });



  server.begin();
  // server.onFileUpload(handleUpload);
}

void loop() {
  ws.cleanupClients();
}