#pragma once

#include <Arduino.h>
#include "ESPAsyncWebServer.h"
#include "ArduinoJson.h"
#include "common.h"

#ifndef DEBUG
#define DEBUG false
#endif
#define debug(msg) Serial.println(msg); 


class websocket_handler
{
private:
    AsyncWebSocket *ws = new AsyncWebSocket("/ws");
public:
    websocket_handler(AsyncWebServer server);
    ~websocket_handler();

    void notify(char *text);
    void onWsEvent(
        AsyncWebSocket       *server,  //
        AsyncWebSocketClient *client,  //
        AwsEventType          type,    //
        void                 *arg,     //
        uint8_t              *data,    //
        size_t                len);    //
    
};