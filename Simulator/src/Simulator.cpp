#include <dlfcn.h>
#include "Simulator.h"
#include <stdexcept>

void Simulator::loadSO(const std::string &path) {
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) throw std::runtime_error("Failed loading so from path: " + path);
    handles.push_back(handle);
}

Simulator::~Simulator(){
    for (auto* handle : handles){
        dlclose(handle);
    }
}