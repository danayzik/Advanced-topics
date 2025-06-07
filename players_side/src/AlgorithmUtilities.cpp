#include "AlgorithmUtilities.h"


ActionRequest rotationTowardsEnemy(const ObservedTank& enemyTank, const ObservedTank& myTank) {
    int enemyX = enemyTank.getX();
    int enemyY = enemyTank.getY();
    int selfY = myTank.getY();
    int selfX = myTank.getX();
    std::optional<Direction> currentDirection = myTank.getDirection();
    if(!currentDirection){
        return ActionRequest::DoNothing;
    }
    Direction targetDirection = getDirectionBetweenPoints(selfY, selfX, enemyY, enemyX);
    if(currentDirection.value() == targetDirection)
        return ActionRequest::DoNothing;
    return getFirstRotationAction(currentDirection.value(), targetDirection);
}

bool enemyInLineOfSight(const FullBattleInfo &battleInfo, const ObservedTank& myTank, size_t targetY, size_t targetX){
    std::optional<Direction> currentDirection = myTank.getDirection();
    if(!currentDirection)
        return false;
    int enemyX = targetX;
    int enemyY = targetY;
    int rows = battleInfo.getRows();
    int cols = battleInfo.getCols();
    auto [dy, dx] = directionToCoordinatesOffset(currentDirection.value());
    const auto& grid = battleInfo.getGrid();
    int selfY = myTank.getY();
    int selfX = myTank.getX();
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

bool hasShellMovingTowardsTank(const FullBattleInfo &battleInfo, const ObservedTank& tank){
    const auto& shellCoordsSet = battleInfo.getShellsCoordinates();
    const auto& grid = battleInfo.getGrid();
    for(auto& shellCoords : shellCoordsSet){
        const ObservedShell& shell = dynamic_cast<const ObservedShell&>(*(grid[shellCoords.yCoord][shellCoords.xCoord].entity));
        std::optional<Direction> shellDirection = shell.getDirection();
        if(!shellDirection)
            continue;
        int shellY = shellCoords.yCoord;
        int shellX = shellCoords.xCoord;
        int tankY = tank.getY();
        int tankX = tank.getX();
        Vec2 delta = {tankY - shellY, tankX - shellX};
        Vec2 dir = Vec2(directionToCoordinatesOffset(shellDirection.value()));
        int cross = delta.x * dir.y - delta.y * dir.x;
        if (cross == 0)
            return true;
    }
    return false;
}

int distanceBetweenTanks(const FullBattleInfo &battleInfo, const ObservedTank& tank1, size_t targetY, size_t targetX){
    int rows = battleInfo.getRows();
    int cols = battleInfo.getCols();
    int firstY = tank1.getY();
    int firstX = tank1.getX();
    int secondY = targetY;
    int secondX = targetX;
    int dy = std::min(abs(secondY - firstY), rows - abs(secondY - firstY));
    int dx = std::min(std::abs(secondX - firstX), cols - std::abs(secondX - firstX));
    return std::max(dy, dx);
}