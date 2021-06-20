#include "ESPAsyncWebServer.h"
#include <wasm3.h>
#include <SPIFFS.h>
#include <stdarg.h>

#define d_m3DoneConst "Execution successful"

class wasm3_module_handler{
private:
    fs::SPIFFSFS *FS;
    File file;

    uint8_t* module;
    size_t size;

    M3Result status;

    IM3Environment env = m3_NewEnvironment();
    IM3Runtime runtime;
    IM3Module wasm_module;
public:
    const String path;
    M3Result result = m3Err_none;
    unsigned output = 0;

    wasm3_module_handler(String filepath, fs::SPIFFSFS *SPIFFS);
    bool load(); // load wasm module from filepath
    bool unload(); // unload module from heap?
    M3Result run(String fName, ...); // run function name


    ~wasm3_module_handler();
};
