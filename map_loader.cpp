#include "map_loader.h"
#include "entities.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
using std::vector;
using std::string;

bool MapLoader::openFile(const string &filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }
    errorLog.open("input_errors.txt");
    return true;
}
bool MapLoader::readDimensions(const string &filePath) {
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

void MapLoader::processMapData(const string &filePath) {
    std::ifstream file(filePath);
    string line;
    int tank1Count = 0;
    int tank2Count = 0;
    int y = 0;
    while (std::getline(file, line)) {
        processRow(line, y, tank1Count, tank2Count);
        y++;
    }
    for (int i = y; i < mapData.rows; ++i) {
        fillMissingRow(y);
    }
}

void MapLoader::processRow(const string &line, int y, int &tank1Count, int &tank2Count) {
    for (int x = 0; x < mapData.cols; ++x) {
        if (x >= line.size()) {
            handleMissingCharacter(y, x);
        } else {
            char cell = line[x];
            handleCell(cell, y, x, tank1Count, tank2Count);
        }
    }
}

//Check constructors
void MapLoader::handleCell(char cell, int y, int x, int &tank1Count, int &tank2Count) {
    switch (cell) {
        case '#':
            mapData.grid[y][x].entitySet.insert(new Wall(y, x, &mapData.grid[y][x]));
            break;
        case '@':
            mapData.grid[y][x].entitySet.insert(new Mine(y, x, &mapData.grid[y][x]));
            break;
        case '1':
            if (tank1Count < 1) {
                Tank* tank = new Tank(y, x, &mapData.grid[y][x]);
                mapData.grid[y][x].entitySet.insert(tank);
                mapData.tank1 = tank;
                tank1Count++;
            }
            break;
        case '2':
            if (tank2Count < 1) {
                Tank* tank = new Tank(y, x, &mapData.grid[y][x]);
                mapData.grid[y][x].entitySet.insert(tank);
                mapData.tank2 = tank;
                tank2Count++;
            }
            break;
        default:
            handleBadCharacter(y, x);
    }
}

void MapLoader::handleBadCharacter(int y, int x) {
    errorLog << "Unexpected character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::handleMissingCharacter(int y, int x) {
    errorLog << "Missing character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::fillMissingRow(int y) {
    for (int x = 0; x < mapData.grid[0].size(); ++x) {
        errorLog << "Missing row " << y << ", filling with empty space.\n";
    }
}

MapData MapLoader::loadMap(const string &filePath) {
    if (!openFile(filePath)) throw std::runtime_error("Failed to open map file: " + filePath);;

    if (!readDimensions(filePath)) throw std::runtime_error("Failed to read dimensions from map file: " + filePath);;

    processMapData(filePath);

    if (errorLog.is_open()) {
        errorLog.close();
    }
    return mapData;
}



