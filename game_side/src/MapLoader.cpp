#include "MapLoader.h"
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <algorithm>
#include <cctype>
#include "GameManager.h"
#include "utils.h"
using namespace EntityUtils;
using std::vector;
using std::string;


MapLoader::MapLoader(const std::string &filePath, GameMap &gameMap, GameManager &gameManager) : filePath(filePath), gameMap(gameMap), gameManager(gameManager){}

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
    std::getline(file, line);
    auto trim = [](std::string& s) {
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
            value = std::stoul(val);
        } catch (const std::exception&) {
            return false;
        }

        if (key == "MaxSteps") maxSteps = value;
        else if (key == "NumShells") numShells = value;
        else if (key == "Rows") rows = value;
        else if (key == "Cols") cols = value;
        else return false;
    }
    gameManager.maxSteps = maxSteps;
    gameManager.numShells = numShells;
    gameMap.rows = rows;
    gameMap.cols = cols;
    gameMap.grid = vector<vector<Cell>>(rows, vector<Cell>(cols));
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            gameMap.grid[y][x].setCoordinates({y, x});
       }
    }
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
    if (cell >= '1' && cell <= '9') {
        int playerNumber = cell - '0';

        if(registeredPlayers.find(playerNumber) == registeredPlayers.end()){
            gameManager.registerPlayer(playerNumber);
            registeredPlayers.insert(playerNumber);
        }
        const Tank &tank = *entityCast<Tank>(
                &gameMap.entityManager.createTank(y, x, playerNumber % 2 == 1 ?  Direction::Left : Direction::Right, playerNumber, tankCount, gameManager.numShells)
        );
        tankCount++;
        gameMap.grid[y][x].entitySet.insert(tank.getEntityId());
        gameMap.tankIds.insert(tank.getEntityId());
        gameManager.registerTank(tank);
        return;
    }
    switch (cell) {
        case ' ':
            return;
        case '#': {
            const GameEntity &entity = gameMap.entityManager.createWall(y, x);
            gameMap.grid[y][x].entitySet.insert(entity.getEntityId());
            return;
        }
        case '@': {
            const GameEntity &entity = gameMap.entityManager.createMine(y, x);
            gameMap.grid[y][x].entitySet.insert(entity.getEntityId());
            return;
        }
        default:
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
    errorBuffer << "Missing row " << y << ", filling with empty space.\n";
}

void MapLoader::loadMap() {
    if (!openFile()) throw std::runtime_error("Failed to open map file: " + filePath);

    if (!readSettings()) throw std::runtime_error("Failed to read settings from map file: " + filePath);

    processMapData();

    if (!errorBuffer.str().empty()) {
        std::ofstream errorLog("input_errors.txt");
        errorLog << errorBuffer.str();
    }
    else{
        std::filesystem::remove("input_errors.txt");
    }

}



