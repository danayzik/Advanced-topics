#pragma once

#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include "game_entity.h"
#include "cell.h"
#include "direction.h"
#include "shell.h"
#include "tank.h"

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


public:
    GameMap(const string& filePath);

    void moveEntity(GameEntity* entity, Direction dir);
    void moveShellsAndCheckCollisions();
};
