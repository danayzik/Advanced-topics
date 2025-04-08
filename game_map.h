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
    unordered_set<Tank*> playerOneTanks;
    unordered_set<Tank*> playerTwoTanks;
    unordered_set<Shell*> shells;
    int rows;
    int cols;
    std::pair<int, int> getNewPosition(const GameEntity* entity, Direction dir) const;
    void resolveCollisions(const unordered_set<Cell*>& dirtyCells);


public:
    explicit GameMap(const string& filePath);
    bool tankCanMoveInDirection(const Tank* tank, Direction dir) const;
    void moveEntity(GameEntity* entity, Direction dir);
    void moveShells();
    void createShell(int y, int x, Direction dir);
    void checkCollisions();
    bool tanksAboutToCollide(const Tank* tank1, const Tank* tank2);
    void shellsAboutToCollide();

};
