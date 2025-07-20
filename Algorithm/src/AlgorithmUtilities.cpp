#include "AlgorithmUtilities.h"

#include "algorithm"
namespace Algorithm_206038929_314620071{
    using namespace EntityUtils;

    std::optional<ActionRequest> rotationTowardsCoords(const ObservedTank &myTank, Coordinates target) {
        int enemyX = target.x;
        int enemyY = target.y;
        int selfY = myTank.getY();
        int selfX = myTank.getX();
        std::optional<Direction> currentDirection = myTank.getDirection();
        if (!currentDirection) {
            return std::nullopt;
        }
        Direction targetDirection = DirectionUtils::getDirectionBetweenPoints(selfY, selfX, enemyY, enemyX);
        if (currentDirection.value() == targetDirection)
            return std::nullopt;
        return getFirstRotationAction(currentDirection.value(), targetDirection);
    }

    bool enemyInLineOfSight(const FullBattleInfo &battleInfo, const ObservedTank &myTank, Coordinates targetCoords) {
        std::optional<Direction> currentDirection = myTank.getDirection();
        if (!currentDirection)
            return false;
        int enemyX = targetCoords.x;
        int enemyY = targetCoords.y;
        int rows = static_cast<int>(battleInfo.getRows());
        int cols = static_cast<int>(battleInfo.getCols());
        auto [dy, dx] = DirectionUtils::directionToCoordinatesOffset(currentDirection.value());
        const auto &grid = battleInfo.getGrid();
        int selfY = myTank.getY();
        int selfX = myTank.getX();
        int currX = selfX;
        int currY = selfY;
        bool blocked = false;
        do {
            currY = (currY + dy + rows) % rows;
            currX = (currX + dx + cols) % cols;
            blocked = !grid[currY][currX].isPassableForShell();
            if (currX == enemyX && currY == enemyY)
                return true;
        } while ((currX != selfX || currY != selfY) && !blocked);
        return false;
    }

    std::optional<Direction>
    directionToShellMovingTowardsTank(const FullBattleInfo &battleInfo, const ObservedTank &tank) {
        int tankY = tank.getY();
        int tankX = tank.getX();
        int rows = static_cast<int>(battleInfo.getRows());
        int cols = static_cast<int>(battleInfo.getCols());
        const auto &shellCoordsSet = battleInfo.getShellsCoordinates();
        for (auto &shellCoords: shellCoordsSet) {
            const auto &shell = *entityCast<ObservedShell>(battleInfo.getCell(shellCoords).entity.get());
            if (!shell.directionKnown())
                continue;
            Direction shellDirection = shell.getDirection().value();
            int shellY = shellCoords.y;
            int shellX = shellCoords.x;
            int dy = (tankY - shellY + rows) % rows;
            if (dy > rows / 2) dy -= rows;
            int dx = (tankX - shellX + cols) % cols;
            if (dx > cols / 2) dx -= cols;
            Vec2 delta = {dy, dx};
            Vec2 dir = Vec2(DirectionUtils::directionToCoordinatesOffset(shellDirection));
            int cross = delta.x * dir.y - delta.y * dir.x;
            if (cross == 0)
                return DirectionUtils::getOppositeDirection(shellDirection);
        }
        return std::nullopt;
    }

    std::optional<ActionRequest> getFirstRotationAction(Direction current, Direction target) {
        int angle = DirectionUtils::getDirectionDiff(current, target);
        if (angle == 0) return std::nullopt;
        if (-90 <= angle && angle <= 90) {
            return angleToRotationAction(angle);
        } else if (angle < -90) {
            return ActionRequest::RotateLeft90;
        } else {
            return ActionRequest::RotateRight90;
        }
    }

    Coordinates getClosestCoordinates(const FullBattleInfo &battleInfo,
                                      const std::unordered_set<Coordinates, CoordinatesHash> &coordsSet) {
        const Coordinates *closest = nullptr;
        size_t rows = battleInfo.getRows();
        size_t cols = battleInfo.getCols();
        Coordinates myTankCoords = battleInfo.getMyTankCoords();
        closest = &(*std::min_element(
                coordsSet.begin(),
                coordsSet.end(),
                [&](const Coordinates &a, const Coordinates &b) {

                    return myTankCoords.distanceToOtherCoord(a, rows, cols) <
                           myTankCoords.distanceToOtherCoord(b, rows, cols);
                }));
        return *closest;
    }

    bool friendlyInDirectionWithinRange(const FullBattleInfo &battleInfo, Direction dir, int range) {
        const auto &friendlyTanks = battleInfo.getFriendlyTanksCoordinates();
        Coordinates myTankCoords = battleInfo.getMyTankCoords();
        Coordinates newPos = myTankCoords;
        for (int i = 0; i < range; ++i) {
            newPos = DirectionUtils::nextCoordinate(dir, newPos, battleInfo.getRows(), battleInfo.getCols());
            if (friendlyTanks.count(newPos))
                return true;
        }
        return false;
    }

    std::optional<Coordinates> getExposedCoordinates(const FullBattleInfo &battleInfo, const ObservedTank &tank,
                                                     const std::unordered_set<Coordinates, CoordinatesHash> &coordsSet) {
        Coordinates tankCoords = tank.getCoords();
        auto rows = battleInfo.getRows();
        auto cols = battleInfo.getCols();
        for (const auto &coords: coordsSet) {
            if (!areColinear(tankCoords, coords, rows, cols))
                continue;
            Direction dirBetween = DirectionUtils::getDirectionBetweenPoints(tankCoords.y, tankCoords.x, coords.y,
                                                                             coords.x);
            std::array<Direction, 2> possibleDirections = {dirBetween,
                                                           DirectionUtils::getOppositeDirection(dirBetween)};
            for (auto dir: possibleDirections) {
                Coordinates nextPos = tankCoords;
                while (true) {
                    nextPos = DirectionUtils::nextCoordinate(dir, nextPos, rows, cols);
                    auto &cell = battleInfo.getCell(nextPos);
                    if (cell.hasEntity() && cell.entity->isWall())
                        break;
                    if (nextPos == coords)
                        return coords;
                }

            }
        }
        return std::nullopt;
    }

    std::optional<Coordinates> getClosestEnemyInLineOfSight(const FullBattleInfo &battleInfo) {
        const auto &enemyCoords = battleInfo.getEnemyTanksCoordinates();
        const ObservedTank &myTank = battleInfo.getMyTank();


        std::unordered_set<Coordinates, CoordinatesHash> visibleEnemies;
        for (const Coordinates &coord: enemyCoords) {
            if (enemyInLineOfSight(battleInfo, myTank, coord)) {
                visibleEnemies.insert(coord);
            }
        }
        if (visibleEnemies.empty())
            return std::nullopt;
        return getClosestCoordinates(battleInfo, visibleEnemies);
    }

    std::optional<ActionRequest> dodgingAction(const FullBattleInfo &battleInfo, const ObservedTank &tank) {
        std::optional<Direction> directionToShellMovingTowardsMeOpt = directionToShellMovingTowardsTank(battleInfo,
                                                                                                        tank);
        if (!directionToShellMovingTowardsMeOpt.has_value())
            return std::nullopt;
        Direction dir = tank.getDirection().value();
        Direction directionToShell = directionToShellMovingTowardsMeOpt.value();
        Coordinates myCoords = tank.getCoords();
        Coordinates coordsInFront = DirectionUtils::nextCoordinate(dir, myCoords, battleInfo.getRows(),
                                                                   battleInfo.getCols());
        auto &cellInFront = battleInfo.getCell(coordsInFront);
        bool movingToShell = dir == directionToShell;
        bool movingAwayFromShell = dir == DirectionUtils::getOppositeDirection(directionToShell);

        bool canMoveForward = cellInFront.isPassableForTank();

        if (canMoveForward && !movingToShell && !movingAwayFromShell)
            return ActionRequest::MoveForward;
        const auto &rotations = getRotationalActions();
        for (auto rotation: rotations) {
            Direction newDirection = DirectionUtils::rotateDirection(dir, rotateActionToAngle(rotation));
            Coordinates coordsAhead = DirectionUtils::nextCoordinate(newDirection, myCoords, battleInfo.getRows(),
                                                                     battleInfo.getCols());
            auto &cellAhead = battleInfo.getCell(coordsAhead);
            if (cellAhead.isPassableForTank())
                return rotation;
        }
        return std::nullopt;

    }

    bool areColinear(const Coordinates &a, const Coordinates &b, size_t rows, size_t cols) {
        int height = static_cast<int>(rows);
        int width = static_cast<int>(cols);
        int dy = (b.y - a.y + height) % height;
        if (dy > height / 2) dy -= height;
        int dx = (b.x - a.x + width) % width;
        if (dx > width / 2) dx -= width;
        if (dx == 0 && dy == 0)
            return true;

        return (dx == 0 || dy == 0 || std::abs(dx) == std::abs(dy));
    }
}