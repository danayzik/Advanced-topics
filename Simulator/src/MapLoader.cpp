#include "MapLoader.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>


using std::vector;
using std::string;
namespace fs = std::filesystem;

bool MapLoader::openFile() {
    file.open(filePath);
    if (!file) {
        std::cerr << "Error opening file: " << filePath << std::endl;
        return false;
    }
    return true;
}

bool MapLoader::readSettings() {
    size_t rows, cols, maxSteps, numShells;
    std::string line;
    size_t value;

    long temp;
    std::getline(file, line);
    auto trim = [](std::string &s) {
        s.erase(std::remove_if(s.begin(), s.end(),
                               [](unsigned char c) { return std::isspace(c); }), s.end());
    };
    for (int i = 0; i < 4; ++i) {
        if (!std::getline(file, line)) return false;
        auto pos = line.find('=');
        if (pos == std::string::npos) return false;
        std::string key = line.substr(0, pos);
        std::string val = line.substr(pos + 1);
        trim(key);
        trim(val);
        try {
            temp = std::stol(val);
            if (temp < 0) {
                std::cerr << "Negative numbers in the map settings are not allowed" << std::endl;
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
    return true;
}

void MapLoader::processMapData() {
    string line;
    size_t y = 0;
    while (std::getline(file, line) && y < gameMap.rows) {
        processRow(line, y);
        y++;
    }
    for (size_t i = y; i < gameMap.rows; ++i) {
        fillMissingRow(y);
    }

}

void MapLoader::processRow(const string &line, size_t y) {
    for (size_t x = 0; x < gameMap.cols; ++x) {
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

// CONCRETE VIEW TO USER COMMON !!!!!!!!!!!!!!!!!!!
void MapLoader::buildView() {
    map.view = std::make_unique<GameManager_206038929_314620071::ConcreteSatelliteView>(map.rows, map.cols, std::move(gameMap));
}

const Map& MapLoader::loadMap(const std::string &mapPath, std::stringstream& simulatorErrorBuffer) {
    gameMap = {};
    mapFilePath = mapPath;
    mapFileName = fs::path(mapPath).filename().string();
    errorBuffer = {};



    if (!openFile()) throw std::invalid_argument("Failed to open map file: " + mapFilePath);

    if (!readSettings())
        throw std::invalid_argument("Failed to read settings from map file: " + mapFilePath + "\n" + settingsExamples);

    processMapData();

    buildView();

    if (!errorBuffer.str().empty()) {
        simulatorErrorBuffer << errorBuffer.str();
    }
    return map;

}





