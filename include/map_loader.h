#pragma once
#include <vector>
#include <string>
#include "cell.h"
#include <utility>
#include "entities.h"
#include <sstream>
#include "game_map.h"
using std::string;


class MapLoader {
private:

    static std::stringstream errorBuffer;
    static bool openFile(const string& filePath, GameMap* gameMap);
    static bool readDimensions(const string& filePath, GameMap* gameMap);
    static void processMapData(const string& filePath, GameMap* gameMap);
    static void processRow(const string& line, int y, int& tank1Count, int& tank2Count, GameMap* gameMap);
    static void handleCell(char cell, int y, int x, int& tank1Count, int& tank2Count, GameMap* gameMap);
    static void handleMissingCharacter(int y, int x);
    static void fillMissingRow(int y);
    static void handleBadCharacter(int y, int x);

public:
    MapLoader() = delete;
    static void loadMap(const std::string& filePath, GameMap* gameMap);

};
