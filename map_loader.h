#pragma once
#include <vector>
#include <string>
#include "cell.h"
#include <utility>
#include "entities.h"
using std::string;

struct MapData {
    int rows;
    int cols;
    std::vector<std::vector<Cell>> grid;
    Tank* tank1;
    Tank* tank2;
};

class MapLoader {
private:
    static MapData mapData;
    static std::ofstream errorLog;
    static bool openFile(const string& filePath);
    static bool readDimensions(const string& filePath);
    static void processMapData(const string& filePath);
    static void processRow(const string& line, int y, int& tank1Count, int& tank2Count);
    static void handleCell(char cell, int y, int x, int& tank1Count, int& tank2Count);
    static void handleMissingCharacter(int y, int x);
    static void fillMissingRow(int y);
    static void handleBadCharacter(int y, int x);

public:
    MapLoader() = delete;
    static MapData loadMap(const std::string& filePath);
};
