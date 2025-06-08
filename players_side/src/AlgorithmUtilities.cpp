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

bool enemyInLineOfSight(const FullBattleInfo &battleInfo, const ObservedTank& myTank, Coordinates targetCoords){
    std::optional<Direction> currentDirection = myTank.getDirection();
    if(!currentDirection)
        return false;
    int enemyX = targetCoords.x;
    int enemyY = targetCoords.y;
    int rows = static_cast<int>(battleInfo.getRows());
    int cols = static_cast<int>(battleInfo.getCols());
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
    for(auto& shellCoords : shellCoordsSet){
        const auto& shell = dynamic_cast<const ObservedShell&>(*(battleInfo.getCell(shellCoords).entity));
        std::optional<Direction> shellDirection = shell.getDirection();
        if(!shellDirection)
            continue;
        int shellY = shellCoords.y;
        int shellX = shellCoords.x;
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

