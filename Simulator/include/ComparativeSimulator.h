#pragma once
#include "Simulator.h"
#include <filesystem>
#include <fstream>
#include "AbstractGameManager.h"
#include "MapLoader.h"


class ComparativeSimulator : public Simulator{
private:
    std::string gameManagersFolder = {};
    std::string mapFileName = {};
    std::atomic<size_t> nextIndex{0};
    std::vector<std::thread> workers;
    Map mapInfo = {};
    int gameManagerCount = {};
    int threads = {};
    bool verbose = false;

    void runGame(AbstractGameManager& manager);

public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;
};