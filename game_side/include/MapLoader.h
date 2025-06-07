#pragma once
#include <vector>
#include <string>
#include "Cell.h"
#include <utility>
#include "Entities.h"
#include <sstream>
#include "GameMap.h"
using std::string;


class MapLoader {
private:
    std::stringstream errorBuffer;
    const std::string& filePath;
    GameMap& gameMap;
    GameManager& gameManager;
    std::ifstream file;
    int tankCount = 0;
    std::unordered_set<int> registeredPlayers = {};
    bool openFile();
    bool readSettings();
    void processMapData();
    void processRow(const string& line, size_t y);
    void handleCell(char cell, size_t y, size_t x);
    void handleMissingCharacter(size_t y, size_t x);
    void fillMissingRow(size_t y);
    void handleBadCharacter(size_t y, size_t x);

public:
    MapLoader(const std::string& filePath, GameMap& gameMap, GameManager& gameManager);
    MapLoader(const MapLoader&) = delete;
    MapLoader(MapLoader&&) = delete;
    MapLoader& operator=(const MapLoader&) = delete;
    MapLoader& operator=(MapLoader&&) = delete;
    ~MapLoader() = default;
    void loadMap();
};
