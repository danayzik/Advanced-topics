#pragma once
#include <vector>
#include <string>
#include <utility>
#include <sstream>
#include <fstream>
#include "ConcreteSatelliteView.h"
#include <memory>
#include <unordered_set>
using std::string;

struct Map{
    size_t maxSteps;
    size_t numShells;
    size_t rows;
    size_t cols;
    std::unique_ptr<SatelliteView> view = {};
};

class MapLoader {
private:
    const std::string settingsExamples = "Example settings (pay attention to capital letters):\n"
                                         "<title>\n"
                                         "MaxSteps=2000\n"
                                         "NumShells=10\n"
                                         "Rows=27\n"
                                         "Cols=29\n"
                                         "<Map data>";
    std::stringstream errorBuffer;
    std::string mapFilePath;
    std::string mapFileName;
    std::ifstream mapFile;
    std::vector<std::vector<char>> gameMap = {};
    int tankCount = 0;


    bool openFile();

    bool readSettings();

    void processMapData();

    void processRow(const string &line, size_t y);

    void handleCell(char cell, size_t y, size_t x);

    void handleMissingCharacter(size_t y, size_t x);

    void fillMissingRow(size_t y);

    void handleBadCharacter(size_t y, size_t x);

    static MapLoader mapLoaderInstance;

    Map map;

    MapLoader() = default;

    std::unordered_set<char> legalChars ={' ', '1', '2', '@', '#'};

    void buildView();

public:

    MapLoader(const MapLoader &) = delete;

    MapLoader(MapLoader &&) = delete;

    MapLoader &operator=(const MapLoader &) = delete;

    MapLoader &operator=(MapLoader &&) = delete;

    ~MapLoader() = default;

    const Map& loadMap(const std::string& mapPath, std::stringstream& simulatorErrorBuffer);

    static MapLoader& getInstance(){return mapLoaderInstance;}
};
