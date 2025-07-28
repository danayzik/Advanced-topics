#include <dlfcn.h>
#include "Simulator.h"
#include <stdexcept>
#include <chrono>
#include <iomanip>
#include <cmath>


void Simulator::loadSO(const std::string &path) {
    void* handle = dlopen(path.c_str(), RTLD_NOW);
    if (!handle) {
        errorBuffer << "Failed loading so from path: " << path << "\n";
        throw std::runtime_error("Failed loading so from path: " + path);
    }
    handles.push_back(handle);
}

Simulator::~Simulator(){
    for (auto* handle : handles){
        dlclose(handle);
    }
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