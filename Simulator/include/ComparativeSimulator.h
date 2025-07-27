#pragma once
#include "Simulator.h"
#include <filesystem>
#include <fstream>
#include "AbstractGameManager.h"
#include "MapLoader.h"
#include "GameResultKey.h"


class ComparativeSimulator : public Simulator{
private:
    std::string gameManagersFolder = {};
    std::string mapFileName = {};
    std::atomic<size_t> nextIndex{0};
    std::vector<std::thread> workers;
    Map mapInfo = {};
    std::unordered_set<size_t> crashedManagersIndices = {};
    std::unordered_map<GameResultKey, std::vector<size_t>> groups;

    std::vector<GameResult> results = {};

    void storeGameResult(GameResult&& result, size_t storeIndex);
    void groupResults();
    void printOutput();
public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;
};

