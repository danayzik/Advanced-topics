#pragma once

#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include "cell.h"
#include "direction.h"
#include "entities.h"

using std::unordered_set;
using std::vector;
using std::string;

class GameMap {
private:
    vector<vector<Cell>> grid;
    unordered_set<Tank*> tanks;
    unordered_set<Shell*> shells;
    int rows;
    int cols;
    std::pair<int, int> getNewPosition(const GameEntity* entity, Direction dir) const;


public:
    explicit GameMap(const string& filePath);
    bool tankCanMoveInDirection(const Tank* tank, Direction dir) const;
    void moveEntity(GameEntity* entity, Direction dir);
    void moveShellsAndCheckCollisions();
};
