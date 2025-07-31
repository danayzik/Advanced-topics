#pragma once
#include "Simulator.h"
#include "Map.h"
class CompetitionSimulator : public Simulator{
private:
    std::string mapsFolder = {};
    std::string algorithmsFolder = {};
    std::vector<Map> maps = {};

    void loadGameManager(const std::string& gameManagerSoPath);
    void loadAlgorithms();
    void loadMaps();
public:
    void loadArguments(const ParsedArguments& arguments) override;
    void run() override;

};