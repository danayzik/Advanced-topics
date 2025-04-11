#include "players.h"
#include "game_map.h"
#include "actions.h"
#include <cstdlib>

using std::abs;
using std::min;

int PlayerV1::distanceToEnemy(const GameMap &gameMap) const {
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    int enemyY = enemyTank->getY();
    int enemyX = enemyTank->getX();
    int selfY = tank->getY();
    int selfX = tank->getX();
    int dy = min(abs(selfY - enemyY), rows - abs(selfY - enemyY));
    int dx = std::min(std::abs(selfX - enemyX), cols - std::abs(selfX - enemyX));
    return std::max(dy, dx);
}

bool PlayerV1::enemyInLineOfSight(const GameMap &gameMap) const {
    Direction currentDirection = tank->getDirection();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    int enemyX = enemyTank->getX();
    int enemyY = enemyTank->getY();
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    auto [dy, dx] = directionToCoordinatesOffset(currentDirection);
    const auto& grid = gameMap.getGrid();
    int selfY = tank->getY();
    int selfX = tank->getX();
    int currX = selfX;
    int currY = selfY;
    bool blocked = false;
    do {
        currY = (currY + dy + rows) % rows;
        currX = (currX + dx + cols) % cols;
        blocked = !grid[currY][currX].isPassableForShell();
        if(currX == enemyX && currY == enemyY)
            return true;
    } while ((currX != selfX || currY != selfY) && !blocked);
    return false;
}

//Super simplified
Action PlayerV1::requestAction(const GameMap &gameMap) {
    bool inLineOfSight = enemyInLineOfSight(gameMap);
    if(inLineOfSight && tank->canFire()){
        return Shoot;
    }
    if(gameMap.tankCanMoveInDirection(tank, tank->getDirection())){
        return MoveForward;
    }
    return RotateRight45;



}
