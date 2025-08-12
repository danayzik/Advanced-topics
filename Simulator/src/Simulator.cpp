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
#include <cstdio>

void Simulator::loadSO(const std::string &path) {
    LOG(LogLevel::INFO, std::string("Attempting to load so: ") + path);
    void* handle = dlopen(path.c_str(), RTLD_NOW | RTLD_GLOBAL);
    if (!handle) {
        const char* err = dlerror();
        errorBuffer << "Failed loading so from path: " << path << "\n";
        errorBuffer << (err ? err : "Unknown error") << "\n";
        LOG(LogLevel::ERROR, std::string("Failed to load so: ") + path);
        throw SharedObjectLoadingException("Failed loading so from path: " + path + "\n");
    }
    LOG(LogLevel::INFO, "Loaded successfully");
    handles.push_back(handle);
}

Simulator::~Simulator(){
    LOG(LogLevel::INFO, "Cleaning up simulator:");
    AlgorithmRegistrar::getAlgorithmRegistrar().clear();
    GameManagerRegistrar::getGameManagerRegistrar().clear();
    LOG(LogLevel::INFO, "Cleaning up so handles");
    for (auto* handle : handles){
        dlclose(handle);
    }
    std::string fileName = "input_errors.txt";
    std::remove(fileName.c_str());
    if(errorBuffer.str().empty()){
        std::cout << "No errors were logged\n";
        return;
    }
    std::ofstream outFile(fileName);
    if (!outFile) {
        std::cerr << "Failed to open file: " << fileName << '\n';
        return;
    }
    outFile << errorBuffer.str();
    outFile.close();
    std::cout << "Written error log to " << fileName << "\n";
}

std::string Simulator::getTimeString() {
    auto now = std::chrono::system_clock::now();
    std::chrono::duration<double> ts = now.time_since_epoch();
    constexpr size_t NUM_DIGITS = 9;
    constexpr size_t NUM_DIGITS_P = 1000000000ULL;
    std::ostringstream oss;
    oss << std::setw(NUM_DIGITS) << std::setfill('0') << size_t(ts.count() * NUM_DIGITS_P) % NUM_DIGITS_P;
    return oss.str();
}

void Simulator::writeResultsToFile(const std::stringstream &resultStream, const std::string &filePrefix,
                                   const std::string &folder) {
    std::string timeStr = getTimeString();
    std::string filename = filePrefix + timeStr + ".txt";
    std::filesystem::path filePath = std::filesystem::path(folder) / filename;
    std::ofstream outFile(filePath);
    LOG(LogLevel::INFO, std::string("Writing output to: ") + filePath.string());
    if (!outFile) {
        throw std::runtime_error("Failed to open file: " + filePath.string());
    }
    outFile << resultStream.str();
    outFile.close();
    std::cout << "Output written to " << filePath.string() << "\n";
}

bool Simulator::validGameResult(const GameResult &result) {
    bool validWinner = result.winner >= 0 && result.winner <= 2;
    bool validRemainingTanksVec = (result.reason == GameResult::MAX_STEPS || result.winner != 0 ) ? result.remaining_tanks.size() == 2 : true;
    bool validGameState = result.gameState != nullptr;
    return validWinner && validGameState && validRemainingTanksVec;
}

void Simulator::loadArguments(const ParsedArguments &arguments) {
    verbose = arguments.verbose;
    threadCount = arguments.numThreads >= 2 ? arguments.numThreads : 0;
}