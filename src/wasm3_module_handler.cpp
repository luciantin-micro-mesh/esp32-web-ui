#include "wasm3_module_handler.h"


wasm3_module_handler::wasm3_module_handler(String filepath, fs::SPIFFSFS *SPIFFS) : path(filepath){
    this->FS = SPIFFS;

    File root = this->FS->open("/");
    File file = root.openNextFile("r");
    Serial.println("-----------START----------");
    Serial.println("Files in SPIFFS in wasm module handler : ");

    while(file){
        Serial.println("-- "+((String)file.name()));
        file = root.openNextFile();
    }

    Serial.println("File path "+filepath+" size : "+this->FS->open(filepath).size());
    Serial.println("-----------END------------");

    // this->path = filepath;
    this->runtime = m3_NewRuntime(this->env, 1024, NULL);

    Serial.println("Created new env for : "+this->path);
}

bool wasm3_module_handler::load(){
    Serial.println("Loading module from path : "+this->path);

    if(this->FS->exists(this->path)){
        this->file = this->FS->open(this->path, "r");

        if(!this->file) return false;
        else{
            Serial.println("-----------START----------");
            Serial.println("Reading Wasm module");
            this->size = this->file.size();
            Serial.println("File size :"+this->size);

            this->module = new uint8_t[this->size];
            this->file.read(this->module, this->size);

            // Serial.println(String(this->module[3]));
            // Serial.println(String(this->module[3],HEX));
            // Serial.println(String(this->module[3],BIN));
            Serial.println("-----------END------------");

            IM3Module module;
            // Serial.println("Module in binary : ");
            // Serial.println(String(this->module[3], BIN));

            if(this->size == 0) return false; // empty file, upload error probably

            result = m3_ParseModule (env, &module, this->module, this->size);
            // if (result) return result;
            // Serial.println(result);
            // Serial.println(ESP.getFreeHeap());
            // Serial.println(ESP.getFreePsram());            
            result = m3_LoadModule (runtime, module);
            // Serial.println(result);
            Serial.println("Module OK !");
            // if (result) return result;
        }
    }
    else return false;

    return true;
}

// FIXME
bool wasm3_module_handler::unload(){
    // delete this->module;
    return true; 
}

M3Result wasm3_module_handler::run(String fName, ...){
    va_list args;
    va_start(args, fName);

    // if(!*this->module) return "File Error";
    
    IM3Function f;
    result = m3_FindFunction (&f, runtime, fName.c_str());
    // Serial.println(result);
    if (result) return result;
       
    result = m3_CallVL(f,args);
    // Serial.println(result);
    if (result) return result;

    unsigned value = 0;
    result = m3_GetResultsV(f, &this->output);
    // Serial.println(result);
    // Serial.println(this->output);
    // if (result) return result;


    va_end(args);
    return d_m3DoneConst;
}

wasm3_module_handler::~wasm3_module_handler(){
    // delete this->module;
}
