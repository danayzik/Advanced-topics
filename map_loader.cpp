#include "map_loader.h"
#include "entities.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
using std::vector;
using std::string;

bool MapLoader::openFile(const std::string &filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }
    errorLog.open("input_errors.txt");
    return true;
}
bool MapLoader::readDimensions(const std::string &filePath) {
    std::ifstream file(filePath);
    int rows, cols;
    file >> rows >> cols;
    file.ignore();

    if (rows <= 0 || cols <= 0) {
        std::cerr << "Invalid dimensions in the input file." << std::endl;
        return false;
    }
    mapData.rows = rows;
    mapData.cols = cols;
    mapData.grid = vector<vector<Cell>>(rows, vector<Cell>(cols));
    return true;
}

void MapLoader::processMapData(const std::string &filePath) {
    std::ifstream file(filePath);
    string line;
    int tank1Count = 0;
    int tank2Count = 0;

    int y = 0;
    while (std::getline(file, line)) {
        processRow(line, y, tank1Count, tank2Count);
        y++;
    }
    for (int y = mapData.grid.size(); y < mapData.grid.size(); ++y) {
        fillMissingRow(y);
    }
}

void MapLoader::processRow(const std::string &line, int y, int &tank1Count, int &tank2Count) {
    for (int x = 0; x < mapData.cols; ++x) {
        if (x >= line.size()) {
            handleMissingCharacter(y, x);
        } else {
            char cell = line[x];
            handleCell(cell, y, x, tank1Count, tank2Count);
        }
    }
}

void MapLoader::handleCell(char cell, int y, int x, int &tank1Count, int &tank2Count) {
    switch (cell) {
        case '#':
            mapData.grid[y][x].entitySet.insert(new Wall());
            break;
        case '@':
            mapData.grid[y][x].entitySet.insert(new Mine());
            break;
        case '1':
            if (tank1Count < 1) {
                Tank* tank = new Tank(nullptr, 0, 0, nullptr, 0, 0);
                mapData.grid[y][x].entitySet.insert(tank); // Give players the tank reference
                mapData.tank1Pos = {y, x};
                tank1Count++;
            }
        case '2':
            if (tank2Count < 1) {
                Tank* tank = new Tank(nullptr, 0, 0, nullptr, 0, 0);
                mapData.grid[y][x].entitySet.insert(tank);
                mapData.tank2Pos = {y, x};
                tank2Count++;
            }
            break;
        default:
            break;
    }
}

void MapLoader::handleMissingCharacter(int y, int x) {
    errorLog << "Missing character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::fillMissingRow(int y) {
    for (int x = 0; x < grid[0].size(); ++x) {
        errorLog << "Missing row " << y << ", filling with empty space.\n";
    }
}

MapData MapLoader::loadMap(const std::string &filePath) {
    if (!openFile(filePath)) return;

    if (!readDimensions(filePath)) return;

    processMapData(filePath);

    if (errorLog.is_open()) {
        errorLog.close();
    }
}



