#include "../include/map_loader.h"
#include "../include/entities.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
using std::vector;
using std::string;


std::stringstream MapLoader::errorBuffer;

bool MapLoader::openFile(const string &filePath) {
    std::ifstream file(filePath);
    if (!file) {
        std::cerr << "Error opening file" << std::endl;
        return false;
    }
    return true;
}
bool MapLoader::readDimensions(const string &filePath, GameMap* gameMap) {
    std::ifstream file(filePath);
    int rows, cols;
    file >> rows >> cols;
    file.ignore();
    if (rows <= 0 || cols <= 0) {
        std::cerr << "Invalid dimensions in the input file." << std::endl;
        return false;
    }
    gameMap->rows = rows;
    gameMap->cols = cols;
    gameMap->grid = vector<vector<Cell>>(rows, vector<Cell>(cols));
    for (int y = 0; y < rows; ++y) {
    for (int x = 0; x < cols; ++x) {
        gameMap->grid[y][x].x = x;
        gameMap->grid[y][x].y = y;
    }
}
    return true;
}

void MapLoader::processMapData(const string &filePath, GameMap* gameMap) {
    std::ifstream file(filePath);
    string line;
    std::getline(file, line);
    int tank1Count = 0;
    int tank2Count = 0;
    int y = 0;
    while (std::getline(file, line) && y < gameMap->rows) {
        processRow(line, y, tank1Count, tank2Count, gameMap);
        y++;
    }
    for (int i = y; i < gameMap->rows; ++i) {
        fillMissingRow(y);
    }
    if(tank1Count == 0 || tank2Count == 0){
        throw std::runtime_error("One or both tanks missing, please add the character '1' and '2' to the map");
    }
}

void MapLoader::processRow(const string &line, int y, int &tank1Count, int &tank2Count, GameMap* gameMap) {
    for (int x = 0; x < gameMap->cols; ++x) {
        if (x >= static_cast<int>(line.size())) {
            handleMissingCharacter(y, x);
        } else {
            char cell = line[x];
            handleCell(cell, y, x, tank1Count, tank2Count, gameMap);
        }
    }
}


void MapLoader::handleCell(char cell, int y, int x, int &tank1Count, int &tank2Count, GameMap* gameMap) {
    switch (cell) {
        case ' ':
            break;
        case '#':
            gameMap->grid[y][x].entitySet.insert(new Wall(y, x, &gameMap->grid[y][x]));
            break;
        case '@':
            gameMap->grid[y][x].entitySet.insert(new Mine(y, x, &gameMap->grid[y][x]));
            break;
        case '1':
            if (tank1Count < 1) {
                Tank* tank = new Tank(y, x, &gameMap->grid[y][x]);
                tank->setOwner(PlayerOne);
                gameMap->grid[y][x].entitySet.insert(tank);
                gameMap->playerOneTanks.insert(tank);
                tank1Count++;
            }
            else{
                errorBuffer << "Duplicate tank '1' at (" << y << ", " << x << "), filling as space.\n";
            }
            break;
        case '2':
            if (tank2Count < 1) {
                Tank* tank = new Tank(y, x, &gameMap->grid[y][x]);
                tank->setOwner(PlayerTwo);
                gameMap->grid[y][x].entitySet.insert(tank);
                gameMap->playerTwoTanks.insert(tank);
                tank2Count++;
            }
            else{
                errorBuffer << "Duplicate tank '2' at (" << y << ", " << x << "), filling as space.\n";
            }
            break;
        default:
            handleBadCharacter(y, x);
    }
}

void MapLoader::handleBadCharacter(int y, int x) {
    errorBuffer << "Unexpected character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::handleMissingCharacter(int y, int x) {
    errorBuffer << "Missing character at (" << y << ", " << x << "), filling as space.\n";
}

void MapLoader::fillMissingRow(int y) {
    errorBuffer << "Missing row " << y << ", filling with empty space.\n";
}

void MapLoader::loadMap(const string &filePath, GameMap* gameMap) {
    if (!openFile(filePath)) throw std::runtime_error("Failed to open map file: " + filePath);

    if (!readDimensions(filePath, gameMap)) throw std::runtime_error("Failed to read dimensions from map file: " + filePath);

    processMapData(filePath, gameMap);

    if (!errorBuffer.str().empty()) {
        std::ofstream errorLog("input_errors.txt");
        errorLog << errorBuffer.str();
    }
    else{
        std::filesystem::remove("input_errors.txt");
    }

}



