#pragma once
#include "Simulator.h"
#include "Map.h"
#include <mutex>
#include <atomic>

class CompetitionSimulator : public Simulator{
private:
    std::string mapsFolder = {};
    std::string algorithmsFolder = {};
    std::string managerFileName = {};
    std::vector<Map> maps = {};
    std::vector<std::pair<std::string, int>> scores = {};
    size_t mapIndexToRun = 0;
    size_t firstAlgoIndexToRun = 0;
    std::mutex errorMutex = {};
    std::mutex indexFetchingMutex = {};
    std::mutex storingResultsMutex = {};
    void loadGameManager(const std::string& gameManagerSoPath);
    void loadAlgorithms();
    void loadMaps();
    void sortScores();
    void storeGameResult(const GameResult& gameResult, size_t algo1Index, size_t algo2Index);
    std::pair<size_t, size_t> fetchIndicesToRun();
    void printOutput();
    void writeResultsToFile(const std::stringstream &ss);
public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;

};