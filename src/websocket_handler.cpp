#include "websocket_handler.h"


websocket_handler::websocket_handler(AsyncWebServer *server, const char *endpoint)
{
    this->ws = new AsyncWebSocket(endpoint);
    // this->ws->onEvent(std::bind1st(websocket_handler::onWsEvent, this));
    this->ws->onEvent(std::bind(&websocket_handler::onWsEvent, this,
     std::placeholders::_1,
     std::placeholders::_2,
     std::placeholders::_3,
     std::placeholders::_4,
     std::placeholders::_5,
     std::placeholders::_6));

    server->addHandler(this->ws);
}


websocket_handler::~websocket_handler()
{
    delete this->ws;
}

void websocket_handler::onWsEvent(
        AsyncWebSocket       *server,  //
        AsyncWebSocketClient *client,  //
        AwsEventType          type,    //
        void                 *arg,     //
        uint8_t              *data,    //
        size_t                len){    //
    
    Serial.println("WS handled...");
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

void websocket_handler::notify(char *text){
    this->ws->textAll(text);
}

void websocket_handler::notify(String text){
    this->ws->textAll(text);
}

void websocket_handler::cleanupClients(){
    this->ws->cleanupClients();
}