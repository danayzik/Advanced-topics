#pragma once
#include "Simulator.h"
#include <filesystem>
#include <fstream>
#include "AbstractGameManager.h"

class ComparativeSimulator : public Simulator{
private:
    std::string gameManagersFolder = {};
    std::string mapFileName = {};
    std::ifstream mapFile = {};
    std::unique_ptr<SatelliteView> startingMapView = {};
    int gameManagerCount = {};
    int threads = {};
    bool verbose = false;

    void runGame(AbstractGameManager& manager);
    void createStartingMapView(const std::string& mapFilePath);

public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;
};