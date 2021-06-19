#include "ESPAsyncWebServer.h"
#include <wasm3.h>

class wasm3_handler {
private:

public:
    wasm3_handler();
    ~wasm3_handler();

    void LoadModule(String path);

};