#include "../include/algorithms.h"
#include <cstdlib>

using std::abs;
using std::min;



bool AlgorithmV1::enemyInLineOfSight(const GameMap &gameMap, const Tank* enemyTank, const Tank* myTank) const {
    Direction currentDirection = myTank->getDirection();
    int enemyX = enemyTank->getX();
    int enemyY = enemyTank->getY();
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    auto [dy, dx] = directionToCoordinatesOffset(currentDirection);
    const auto& grid = gameMap.getGrid();
    int selfY = myTank->getY();
    int selfX = myTank->getX();
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
Action AlgorithmV1::getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) {
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    bool inLineOfSight = enemyInLineOfSight(gameMap, enemyTank, myTank);
    if(inLineOfSight && myTank->canFire()){
        return Shoot;
    }
    if(gameMap.tankCanMoveInDirection(myTank, myTank->getDirection())){
        return MoveForward;
    }
    return RotateRight45;




}
