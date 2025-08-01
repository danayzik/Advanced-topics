#pragma once
#include "Simulator.h"
#include <filesystem>
#include <fstream>
#include "AbstractGameManager.h"
#include "MapLoader.h"
#include "GameResultKey.h"
#include <thread>
#include <atomic>
#include <mutex>
class ComparativeSimulator : public Simulator{
private:
    std::string gameManagersFolder = {};
    std::string mapFileName = {};
    std::atomic<size_t> nextIndex{0};
    Map mapInfo = {};
    std::unordered_set<size_t> crashedManagersIndices = {};
    std::unordered_map<GameResultKey, std::vector<size_t>> groups{};
    std::mutex errorMutex{};
    std::mutex storingResultsMutex = {};
    std::vector<GameResult> results = {};


    void loadAlgorithms(const ParsedArguments& arguments);
    void loadGameManagers();
    void loadMapFile(const std::string& path);
    void printGameResult(const GameResult& result, std::stringstream& buffer);
    void storeGameResult(GameResult&& result, size_t storeIndex);
    void writeResultsToFile(const std::stringstream &ss);
    void groupResults();
    void printOutput();
public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;
    ~ComparativeSimulator() override = default;
};

