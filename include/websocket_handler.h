#include "ESPAsyncWebServer.h"

class websocket_handler
{
private:
    AsyncWebSocket *ws;
public:
    websocket_handler(AsyncWebServer *server, const char *endpoint);
    ~websocket_handler();

    void notify(char *text);
    void notify(String text);

    void onWsEvent(
        AsyncWebSocket       *server,  //
        AsyncWebSocketClient *client,  //
        AwsEventType          type,    //
        void                 *arg,     //
        uint8_t              *data,    //
        size_t                len);    //

    void cleanupClients();
};
