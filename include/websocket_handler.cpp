#include "websocket_handler.h"
#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"
#include <Arduino.h>
#include <SPIFFS.h>
#include <WiFi.h>
#include "ESPAsyncWebServer.h"
#include "wifi_settings.h"
#include "ArduinoJson.h"
#include <wasm3.h>
#include <m3_env.h>
/*
//  WS data
//  const char * SPIFFS_files []
//  String WASM_MODULES []
//  unsigned int size_t SPIFFS free bytes
//  
*/

// AwsEventHandler handler{

// }

void websocket_handler::onWsEvent(
        AsyncWebSocket       *server,  //
        AsyncWebSocketClient *client,  //
        AwsEventType          type,    //
        void                 *arg,     //
        uint8_t              *data,    //
        size_t                len){    //

    
}


void onWssEvent(AsyncWebSocket       *server,  //
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


websocket_handler::websocket_handler(AsyncWebServer server)
{
    // // this->ws->onEvent(onWssEvent);
    // std::function<void(AsyncWebSocket       *server,  //
    //     AsyncWebSocketClient *client,  //
    //     AwsEventType          type,    //
    //     void                 *arg,     //
    //     uint8_t              *data,    //
    //     size_t                len)> fn = (void)this.onWsEvent;
    // AwsEventHandler tst = this->onWsEvent();
    
    this->ws->onEvent(std::bind1st(websocket_handler::onWsEvent, this));
    server.addHandler(this->ws);
}

websocket_handler::~websocket_handler()
{
}

void websocket_handler::notify(char *text){
    this->ws->textAll(text);
}


// void notifyWs(void *pvParameters){
//   vTaskDelay(1000);
//   (void) pvParameters;

//   for(;;){
//     ws.textAll("Hello World!");
//     vTaskDelay(100);
//   }
// }



void initWebSocket() {

}