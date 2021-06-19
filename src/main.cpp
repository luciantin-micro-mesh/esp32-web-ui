#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>

#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"

#include <wasm3.h>
#include "wasm3_module_handler.h"
#include "wasm_examples/fib.h"

#include "wifi_settings.h"
#include "websocket_handler.h"


// Network Settings
// Build flags || wifi_settings.h
// Wifi passwd and SSID :
// 1. add wifi_settings.h file inside the include dir or uncomment the defines and comment the include
// 2. add wifi passwd &  SSID
//#define NET_SSID "****"
//#define NET_PWD "****"

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

struct wasm_server
{
  wasm3_module_handler *module;
  AsyncWebServer *server;
  int port;
};

int WASM_SERVER_PORT_COUNTER = 100;

AsyncWebServer server(80);
AsyncWebServer wast(81);
websocket_handler *ws;
std::vector<wasm_server*> WASM_MODULES;


void initWiFi() {
  WiFi.begin(NET_SSID, NET_PWD);
  Serial.printf("Trying to connect [%s] ", WiFi.macAddress().c_str());
  while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(200);
  }
  Serial.printf("\nLocal IP : %s\n", WiFi.localIP().toString().c_str());
}

void NotifyWS(void *pvParameters){
  (void) pvParameters;
  for(;;){
    ws->notify("Hello World !");
    vTaskDelay(100);
  }
}

wasm_server* create_wasm_server(String filepath){
      Serial.println("Creating server for file : "+filepath);
      wasm_server *tmp = new wasm_server;
      tmp->module = new wasm3_module_handler(filepath, &SPIFFS);
      tmp->port = WASM_SERVER_PORT_COUNTER++;
      tmp->server = new AsyncWebServer(tmp->port);

      tmp->server->begin();

      tmp->module->load();
      
      Serial.println("Running function ...");

      M3Result res = tmp->module->run("fun");
      Serial.println(res);
      // if(res != d_m3DoneConst)  request->send(200, "text/plain", "Error running function");    
      

      unsigned output = tmp->module->output;
      Serial.println(output);
      // request->send(200, "text/plain", String(output));


      tmp->server->on("^(.*?)$", HTTP_GET, [](AsyncWebServerRequest *request){
        // tmp->module->load();
        
        // Serial.println("Running function ...");

        // M3Result res = tmp->module->run("fib", 6);
        // if(res != d_m3DoneConst)  request->send(200, "text/plain", "Error running function");    
        
        // unsigned output = tmp->module->output;

        // request->send(200, "text/plain", String(output));    
        request->send(200, "text/plain", "asdasdasd");    
      });

      return tmp;
}

void handleUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    Serial.println((String)"UploadStart: " + filename);
    request->_tempFile = SPIFFS.open("/"+filename, "w");

    if(filename.endsWith("wasm")){
      WASM_MODULES.push_back(create_wasm_server("/"+filename));
    }
  }
  if(len) {
    // for(size_t i=0; i<len; i++){
    //   Serial.write(data[i]);
    // }
    request->_tempFile.write(data,len);
  }
  if(final){
    Serial.println((String)"UploadEnd: " + filename + "," + index+len);
    request->_tempFile.close();
    request->send(200, "text/plain", "File Uploaded !");
  }
}


void scanForWasm(){
  File root = SPIFFS.open("/");
  File file = root.openNextFile("r");
  Serial.println("Loading WASM modules from FS ...");

  while(file){
    if(((String)file.name()).endsWith("wasm")){
      Serial.println("Loading : "+((String)file.name()));
      WASM_MODULES.push_back(create_wasm_server(((String)file.name())));
    }
    
    file = root.openNextFile();
  }
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(FLASH_PIN, OUTPUT);


  Serial.begin(115200);
  delay(10);
  Serial.println("Init .....");
  WiFi.mode(WIFI_STA);


  // initWiFi();
  initSPIFFS();
  // ws = new websocket_handler(&server, "/ws");

  // // Serial.println(run_wasm());

  // // scanForWasm();

  // // xTaskCreatePinnedToCore(NotifyWS, "NotifyWS", 1024, NULL, 2, NULL, CORE_2);
  // // ws.notify("Test");

  // server.on("/doUpload", HTTP_POST, [](AsyncWebServerRequest *request) {},
  //     [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data,
  //                   size_t len, bool final) {handleUpload(request, filename, index, data, len, final);}
  // );

  // /*
  // //  WS data
  // //  const char * SPIFFS_files []
  // //  String WASM_MODULES []
  // //  unsigned int size_t SPIFFS free bytes
  // //  
  // */

  // server.on("/metrics", HTTP_GET, [](AsyncWebServerRequest *request) {
  //   StaticJsonDocument<600> data;
  //   JsonObject JSON_Root = data.to<JsonObject>();
  //   JsonArray Files = JSON_Root.createNestedArray("files");
  //   JsonObject Ports = JSON_Root.createNestedObject("ports");
  //   // JsonArray WASM = JSON_Root.createNestedArray("wasm");

  //   data["Used bytes"] = SPIFFS.usedBytes();

  //   File root = SPIFFS.open("/");
  //   File file = root.openNextFile("r");

  //   while(file){
  //     Files.add(String((const __FlashStringHelper*) file.name()));
  //     file = root.openNextFile();
  //   }

  //   // if(WASM_MODULES.size() > 0){
  //   for(auto & server : WASM_MODULES){
  //     Ports[server->module->path] = server->port;
  //   }
  //   // }

  //   String response;
  //   serializeJson(data, response);
  //   request->send(200, "application/json", response);
  // });

  // server.serveStatic("/", SPIFFS, "/").setDefaultFile("index.html");


//
  // server.on("^(.*?)$", HTTP_GET, [](AsyncWebServerRequest *request){

  // wast.on("^(.*?)$", HTTP_GET, [](AsyncWebServerRequest *request){
  //   request->send(200, "text/plain", String(run_wasm()));    
  // });

  // server.begin();
  // wast.begin();


  File file = SPIFFS.open("/testing.wasm", "r");
  
  if(!file) Serial.println("error Reading file");
  else{
        // String s=file.readString();
        size_t size = file.size();
        Serial.println("Size : "+String(size));
        char buf[size];
        int siz = file.readBytes(buf,size);
        // Serial.println("File :");
        // Serial.println(s);


        Serial.println("File :");
        Serial.println("Size : "+ String(siz)); 

        Serial.println("HERE !");
      // uint8_t module [int(size)];
      // file.read(module, size);
      // for(int i = 0; i<10;i++) Serial.println(String(module[i],HEX));
      // Serial.println(String(*module));
      // Serial.println(String(*module,HEX));
      // Serial.println(String(*module,BIN));
  }

  Serial.println("HERE ??");
  // Serial.println("First :"+fib32_wasm[40]);
}

void loop() {
  // ws->cleanupClients();
}