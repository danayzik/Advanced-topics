#pragma once
#include "Simulator.h"
#include "Map.h"
#include <mutex>
#include <atomic>

class CompetitionSimulator : public Simulator{
private:
    std::string mapsFolder = {};
    std::string algorithmsFolder = {};
    std::vector<Map> maps = {};
    std::vector<int> scores = {};
    size_t mapIndexToRun = 0;
    size_t firstAlgoIndexToRun = 0;
    std::mutex errorMutex = {};
    std::mutex indexFetchingMutex = {};
    std::mutex storingResultsMutex = {};
    void loadGameManager(const std::string& gameManagerSoPath);
    void loadAlgorithms();
    void loadMaps();
    void storeGameResult(const GameResult& gameResult, size_t algo1Index, size_t algo2Index);
    std::pair<size_t, size_t> fetchIndicesToRun();
public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;

};