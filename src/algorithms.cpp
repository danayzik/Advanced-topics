#include "../include/algorithms.h"

ActionRequest Algorithm::rotationTowardsEnemy(const Tank *enemyTank, const Tank *myTank) {
    int enemyX = enemyTank->getX();
    int enemyY = enemyTank->getY();
    int selfY = myTank->getY();
    int selfX = myTank->getX();
    Direction currentDirection = myTank->getDirection();
    Direction targetDirection = getDirectionBetweenPoints(selfY, selfX, enemyY, enemyX);
    if(currentDirection == targetDirection)
        return DoNothing;
    return getFirstRotationAction(currentDirection, targetDirection);
}

int Algorithm::distanceBetweenTanks(const GameMap &gameMap, const Tank *tank1, const Tank *tank2) {
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    int firstY = tank1->getY();
    int firstX = tank1->getX();
    int secondY = tank2->getY();
    int secondX = tank2->getX();
    int dy = std::min(abs(secondY - firstY), rows - abs(secondY - firstY));
    int dx = std::min(std::abs(secondX - firstX), cols - std::abs(secondX - firstX));
    return std::max(dy, dx);
}

bool Algorithm::enemyInLineOfSight(const GameMap &gameMap, const Tank *enemyTank, const Tank *myTank) {
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

bool Algorithm::hasShellMovingTowardsTank(const GameMap &gameMap, const Tank *tank) {
    const auto& shells = gameMap.getShells();
    for(Shell* shell : shells){
        Vec2 delta = {tank->getY() - shell->getY(), tank->getX() - shell->getX()};
        Vec2 dir = Vec2(directionToCoordinatesOffset(shell->getDirection()));
        int cross = delta.x * dir.y - delta.y * dir.x;
        if (cross == 0)
            return true;
    }
    return false;
}