#include "wasm3_module_handler.h"


wasm3_module_handler::wasm3_module_handler(String filepath, fs::SPIFFSFS *SPIFFS) : path(filepath){
    this->FS = SPIFFS;
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
            this->size = this->file.size();
            this->file.read(this->module, this->size);
            Serial.println(String(*this->module));
            Serial.println(String(*this->module,HEX));
            Serial.println(String(*this->module,BIN));
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
    
    IM3Module module;
    Serial.println(*this->module);
    result = m3_ParseModule (env, &module, this->module, this->size);
    if (result) return result;

    result = m3_LoadModule (runtime, module);
    if (result) return result;

    IM3Function f;
    result = m3_FindFunction (&f, runtime, fName.c_str());
    if (result) return result;
       
    result = m3_CallVL(f,args);
    if (result) return result;

    result = m3_GetResultsV (f, this->output);
    if (result) return result;


    va_end(args);
    return d_m3DoneConst;
}

wasm3_module_handler::~wasm3_module_handler(){
    delete this->module;
}
