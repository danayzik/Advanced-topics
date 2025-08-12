#include "MapLoader.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include "Exceptions.h"
#include "Logger.h"

using std::vector;
using std::string;
namespace fs = std::filesystem;
MapLoader MapLoader::mapLoaderInstance;

bool MapLoader::openFile() {
    LOG(LogLevel::INFO, std::string("Attempting to open map file: ") + mapFilePath);
    mapFile.open(mapFilePath);
    if (!mapFile) {
        return false;
    }
    LOG(LogLevel::INFO, "Opened successfully");
    return true;
}

bool MapLoader::readSettings() {
    LOG(LogLevel::INFO, std::string("Attempting to read settings from: ") + mapFilePath);
    size_t rows, cols, maxSteps, numShells;
    std::string line;
    size_t value;
    long temp;
    std::getline(mapFile, line);
    auto trim = [](std::string &s) {
        s.erase(std::remove_if(s.begin(), s.end(),
                               [](unsigned char c) { return std::isspace(c); }), s.end());
    };
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(mapFile, line)) return false;
        auto pos = line.find('=');
        if (pos == std::string::npos) return false;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        trim(key);
        trim(val);
        try {
            temp = std::stol(val);
            if (temp < 0) {
                return false;
            }
            value = static_cast<size_t>(temp);
        } catch (const std::exception &) {

            return false;
        }

        if (key == "MaxSteps") maxSteps = value;
        else if (key == "NumShells") numShells = value;
        else if (key == "Rows") rows = value;
        else if (key == "Cols") cols = value;
        else return false;
    }
    map.maxSteps = maxSteps;
    map.numShells = numShells;
    map.rows = rows;
    map.cols = cols;
    gameMap = std::vector<std::vector<char>>(rows, std::vector<char>(cols, ' '));
    LOG(LogLevel::INFO, "Settings read successfully");
    return true;
}

void MapLoader::processMapData() {
    LOG(LogLevel::INFO, std::string("Processing map data of: ") + mapFilePath);
    string line;
    size_t y = 0;
    while (std::getline(mapFile, line) && y < map.rows) {
        processRow(line, y);
        y++;
    }
    for (size_t i = y; i < map.rows; ++i) {
        fillMissingRow(y);
    }
    LOG(LogLevel::INFO, std::string("Finished processing map data of: ") + mapFilePath);

}

void MapLoader::processRow(const string &line, size_t y) {
    for (size_t x = 0; x < map.cols; ++x) {
        if (x >= static_cast<size_t>(line.size())) {
            handleMissingCharacter(y, x);
        } else {
            char cell = line[x];
            handleCell(cell, y, x);
        }
    }
}


void MapLoader::handleCell(char cell, size_t y, size_t x) {
    if (legalChars.count(cell)) {
        gameMap[y][x] = cell;
    }
    else{
        handleBadCharacter(y, x);
    }
}

void MapLoader::handleBadCharacter(size_t y, size_t x) {
    errorBuffer << "Unexpected character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::handleMissingCharacter(size_t y, size_t x) {
    errorBuffer << "Missing character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::fillMissingRow(size_t y) {
    errorBuffer << "Missing row " << y << ", filling with empty spaces.\n";
}


void MapLoader::buildView() {
    LOG(LogLevel::INFO, std::string("Attempting to build map SatelliteView for file: ") + mapFilePath);
    map.view = std::make_unique<UserCommon_206038929_314620071::ConcreteSatelliteView>(map.rows, map.cols, std::move(gameMap));
    LOG(LogLevel::INFO, "Finished building SatelliteView");
}

Map MapLoader::loadMap(const std::string &mapPath, std::stringstream& simulatorErrorBuffer) {
    gameMap = {};
    mapFilePath = mapPath;
    map.mapFileName = fs::path(mapPath).filename().string();
    errorBuffer = {};
    std::string titleString = map.mapFileName + " errors:\n";
    errorBuffer << titleString;
    LOG(LogLevel::INFO, std::string("Attempting to load map file: ") + mapFilePath);


    if (!openFile()) {
        cleanUp(simulatorErrorBuffer, titleString);
        throw MapLoadingException("Failed to open map file: " + mapFilePath);
    }

    if (!readSettings()) {
        cleanUp(simulatorErrorBuffer, titleString);
        throw MapLoadingException("Failed to read settings from map file: " + mapFilePath + "\n" + settingsExamples);
    }
    processMapData();

    buildView();

    cleanUp(simulatorErrorBuffer, titleString);
    LOG(LogLevel::INFO, std::string("Finished loading map: ") + mapFilePath);
    return std::move(map);

}

void MapLoader::cleanUp(std::stringstream& simulatorErrorBuffer, const std::string& titleString) {
    if (errorBuffer.str() != titleString) {
        simulatorErrorBuffer << errorBuffer.str();
    }
    if (mapFile.is_open()) {
        mapFile.close();
    }
}





