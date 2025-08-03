#include <dlfcn.h>
#include "Simulator.h"
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <cmath>
#include "Exceptions.h"
#include <filesystem>
#include <iostream>
#include <fstream>
#include "AlgorithmRegistrar.h"
#include "GameManagerRegistrar.h"

void Simulator::loadSO(const std::string &path) {
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        const char* err = dlerror();
        errorBuffer << "Failed loading so from path: " << path << "\n";
        std::cerr << "dlopen failed: " << (err ? err : "Unknown error") << "\n";
        throw SharedObjectLoadingException("Failed loading so from path: " + path);
    }

    handles.push_back(handle);
}

Simulator::~Simulator(){
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    for (auto* handle : handles){
        dlclose(handle);
    }
    if(errorBuffer.str().empty()){
        return;
    }
    std::string fileName = "input_errors.txt";
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "Failed to open file: " << fileName << '\n';
        return;
    }
    outFile << errorBuffer.str();
    outFile.close();
}

std::string Simulator::getTimeString() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> ts = now.time_since_epoch();
    constexpr size_t NUM_DIGITS = 9;
    size_t NUM_DIGITS_P = std::pow(10, NUM_DIGITS);
    std::ostringstream oss;
    oss << std::setw(NUM_DIGITS) << std::setfill('0') << size_t(ts.count() * NUM_DIGITS_P) % NUM_DIGITS_P;
    return oss.str();
}

void Simulator::loadArguments(const ParsedArguments &arguments) {
    verbose = arguments.verbose;
    threadCount = arguments.numThreads >= 2 ? arguments.numThreads : 0;
}