#include "AlgorithmUtilities.h"
#include "utils.h"
#include "algorithm"
using namespace EntityUtils;

std::optional<ActionRequest> rotationTowardsCoords(const ObservedTank& myTank, Coordinates target) {
    int enemyX = target.x;
    int enemyY = target.y;
    int selfY = myTank.getY();
    int selfX = myTank.getX();
    std::optional<Direction> currentDirection = myTank.getDirection();
    if(!currentDirection){
        return std::nullopt;
    }
    Direction targetDirection = getDirectionBetweenPoints(selfY, selfX, enemyY, enemyX);
    if(currentDirection.value() == targetDirection)
        return std::nullopt;
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
        const auto& shell = *entityCast<ObservedShell>(battleInfo.getCell(shellCoords).entity.get());
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

std::optional<ActionRequest> getFirstRotationAction(Direction current, Direction target) {
    int angle = getDirectionDiff(current, target);
    if (angle == 0) return std::nullopt;
    if (-90 <= angle && angle <= 90) {
        return angleToRotationAction(angle);
    } else if (angle < -90) {
        return ActionRequest::RotateLeft90;
    } else {
        return ActionRequest::RotateRight90;
    }
}

Coordinates getClosestCoordinates(const FullBattleInfo &battleInfo, const std::unordered_set<Coordinates, CoordinatesHash>& coordsSet) {
    const Coordinates* closest = nullptr;
    size_t rows = battleInfo.getRows();
    size_t cols = battleInfo.getCols();
    Coordinates myTankCoords = battleInfo.getMyTankCoords();
    closest = &(*std::min_element(
            coordsSet.begin(),
            coordsSet.end(),
            [&](const Coordinates& a, const Coordinates& b) {

                return myTankCoords.distanceToOtherCoord(a, rows, cols) <
                       myTankCoords.distanceToOtherCoord(b, rows, cols);
            }));
    return *closest;
}

bool friendlyInDirectionWithinRange(const FullBattleInfo &battleInfo, Direction dir, int range){
    const auto& friendlyTanks = battleInfo.getFriendlyTanksCoordinates();
    Coordinates myTankCoords = battleInfo.getMyTankCoords();
    Coordinates newPos = myTankCoords;
    for (int i = 0; i < range; ++i) {
        newPos = nextCoordinate(dir, newPos, battleInfo.getRows(), battleInfo.getCols());
        if(friendlyTanks.count(newPos))
            return true;
    }
    return false;
}

std::optional<Coordinates> getClosestEnemyInLineOfSight(const FullBattleInfo &battleInfo){
    const auto& enemyCoords = battleInfo.getEnemyTanksCoordinates();
    const ObservedTank& myTank = battleInfo.getMyTank();

    std::unordered_set<Coordinates, CoordinatesHash> visibleEnemies;
    for (const Coordinates& coord : enemyCoords) {
        if (enemyInLineOfSight(battleInfo, myTank, coord)) {
            visibleEnemies.insert(coord);
        }
    }
    if(visibleEnemies.empty())
        return std::nullopt;
    return getClosestCoordinates(battleInfo, visibleEnemies);
}
