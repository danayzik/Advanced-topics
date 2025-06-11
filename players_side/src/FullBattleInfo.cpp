#include "FullBattleInfo.h"
#include "utils.h"

using namespace EntityUtils;
FullBattleInfo::FullBattleInfo() : rows(0), cols(0), startingShells(0), observedCells{}, playerIndex(0) {}

FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, int playerIndex) : rows(rows), cols(cols), startingShells(startingShells), observedCells(rows, std::vector<ObservedCell>(cols)), playerIndex(playerIndex)  {}


FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView &satelliteView,
                               int playerIndex) : rows(rows), cols(cols), startingShells(startingShells), observedCells(rows, std::vector<ObservedCell>(cols)), playerIndex(playerIndex){
    Direction myTankDirection = playerIndex % 2 == 1 ?  Direction::Left : Direction::Right;
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            char symbol = satelliteView.getObjectAt(x, y);
            if (symbol >= '1' && symbol <= '9') {
                int playerNumber = symbol - '0';
                bool friendly = playerNumber == playerIndex;
                observedCells[y][x].setEntity(std::make_unique<ObservedTank>(y, x, playerNumber, startingShells, playerNumber % 2 == 1 ? Direction::Left : Direction::Right));
                if(friendly)
                    friendlyTanksCoordinates.insert({y, x});
                else
                    enemyTanksCoordinates.insert({y, x});
                continue;
            }
            switch (symbol){
                case '%':
                    myTankCoords = {y, x};
                    observedCells[y][x].setEntity(std::make_unique<ObservedTank>(myTankCoords, playerIndex, startingShells, myTankDirection));
                    friendlyTanksCoordinates.insert(myTankCoords);
                    break;
                case '@':
                    observedCells[y][x].setMine();
                    break;
                case '#':
                    observedCells[y][x].setEntity(std::make_unique<ObservedWall>(y, x));
                    break;
                case ' ':
                    break;
                case '*':
                    observedCells[y][x].setEntity(std::make_unique<ObservedShell>(y, x));
                    shellCoordinates.insert({y, x});
                    break;
                default:
                    throw std::runtime_error("Out of bounds or incorrect symbol");
            }
        }
    }
}

void FullBattleInfo::updateFromEarlierInfo(FullBattleInfo &earlierInfo) {
    requestInfoCountdown = earlierInfo.requestInfoCountdown;
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            auto& oldCell = earlierInfo.getCell(y, x);
            auto& newCell = getCell(y, x);
            bool bothHaveEntities = oldCell.hasEntity() && newCell.hasEntity();
            if(oldCell.hasMine && newCell.hasEntity() && newCell.entity->getType() == EntityType::Shell){
                newCell.setMine();
            }
            if(bothHaveEntities && (oldCell.entity->getType() == newCell.entity->getType())){
                if(oldCell.entity->getType() == EntityType::Tank){
                    Coordinates coords = {y, x};
                    bool oldIsMyTank = earlierInfo.friendlyTanksCoordinates.count(coords);
                    bool newIsMyTank = friendlyTanksCoordinates.count(coords);
                    if(oldIsMyTank ^ newIsMyTank){
                        if(oldIsMyTank){
                            friendlyTanksCoordinates.insert(coords);
                            enemyTanksCoordinates.erase(coords);
                        }
                        else{
                            friendlyTanksCoordinates.erase(coords);
                            enemyTanksCoordinates.insert(coords);
                        }
                    }
                }
                newCell.setEntity(std::move(oldCell.entity));
            }

        }
    }

}

void FullBattleInfo::moveEntity(Coordinates from, Coordinates to) {

    auto& sourceCell = getCell(from);
    ObservedEntity& entity = *sourceCell.entity;
    auto& targetCell = getCell(to);
    if(myTankCoords == to){
        sourceCell.clearEntity();
        shellCoordinates.erase(from);
        friendlyTanksCoordinates.erase(from);
        enemyTanksCoordinates.erase(from);
        return;
    }
    if(targetCell.hasEntity() && to != myTankCoords){
        EntityType type = targetCell.entity->getType();
        switch (type) {
            case EntityType::Shell:
                shellCoordinates.erase(to);
                break;
            case EntityType::Tank:
                friendlyTanksCoordinates.erase(to);
                enemyTanksCoordinates.erase(to);
                break;
            default:
                break;
        }
    }
    if (from == myTankCoords)
        myTankCoords = to;
    entity.setCoords(to);
    EntityType entityType = entity.getType();
    targetCell.setEntity(std::move(sourceCell.entity));
    switch (entityType) {
        case EntityType::Shell:
            shellCoordinates.erase(from);
            shellCoordinates.insert(to);
            break;
        case EntityType::Tank:
            if(dynamic_cast<ObservedTank&>(entity).ownedByPlayer(playerIndex)){
                friendlyTanksCoordinates.erase(from);
                friendlyTanksCoordinates.insert(to);
            }
            else{
                enemyTanksCoordinates.erase(from);
                enemyTanksCoordinates.insert(to);
            }

            break;
        default:
            break;

    }
}

void FullBattleInfo::fireShellFromTank(ObservedTank &tank) {

    if(!tank.directionKnown())
        return;
    Direction dir = tank.getDirection().value();
    Coordinates coords = tank.getCoords();
    Coordinates newCoords = DirectionUtils::nextCoordinate(dir, coords, rows, cols);
    auto& cell = getCell(newCoords);
    if(cell.hasEntity() && (cell.entity->isWall() || cell.entity->isTank())) {
        if (cell.entity->isTank()){
            cell.clearEntity();
            if(newCoords != myTankCoords)
                friendlyTanksCoordinates.erase(newCoords);
            enemyTanksCoordinates.erase(newCoords);
        }
        return;
    }
    newCoords = DirectionUtils::nextCoordinate(dir, newCoords, rows, cols);
    auto& nextCell = getCell(newCoords);
    if(nextCell.hasEntity() && (nextCell.entity->isWall() || nextCell.entity->isTank())) {
        if (nextCell.entity->isTank()) {
            nextCell.clearEntity();
            if(newCoords != myTankCoords)
                friendlyTanksCoordinates.erase(newCoords);
            enemyTanksCoordinates.erase(newCoords);
        }
        return;
    }
    nextCell.setEntity(std::make_unique<ObservedShell>(newCoords, dir, true));
    shellCoordinates.insert(newCoords);
}

void FullBattleInfo::updateFromTankInfo(FullBattleInfo &tankInfo) {
    tankIndex = tankInfo.getTankIndex();
    auto tankCoords = tankInfo.getMyTankCoords();

    auto& tankCell = getCell(tankCoords);
    if(tankCell.hasEntity() && tankCell.entity->getType() == EntityType::Tank){
        tankCell.setEntity(std::move(tankInfo.getCell(tankCoords).entity));
    }
    else{
        throw std::runtime_error("Didn't follow your tank correctly");
    }
    auto& shellCoordsFromTankInfo = tankInfo.getShellsCoordinates();
    for(auto coord : shellCoordsFromTankInfo){
        auto& shellPotentialCell = getCell(coord);
        if(shellPotentialCell.hasEntity() && shellPotentialCell.entity->isShell()){
            auto& shell = *entityCast<ObservedShell>(shellPotentialCell.entity.get());
            auto& shellFromTankInfo = *entityCast<ObservedShell>(tankInfo.getCell(coord).entity.get());
            if(shellFromTankInfo.directionKnown()){
                shell.setDirection(shellFromTankInfo.getDirection().value());

            }
        }
    }

}


void FullBattleInfo::moveMyTankForward() {
    auto& myTank = getMyTank();
    Coordinates oldCoords = myTank.getCoords();
    Coordinates newCoords = DirectionUtils::nextCoordinate(myTank.getDirection().value(), myTankCoords, rows, cols);
    moveEntity(oldCoords, newCoords);


}

void FullBattleInfo::setTankIndex(int index) {
    tankIndex = index;
    auto& myTank = getMyTank();
    myTank.setIndex(index);
}

void FullBattleInfo::moveKnownShells() {
    std::unordered_set<Coordinates, CoordinatesHash> oldShellCoords = shellCoordinates;
    for(auto coord : oldShellCoords){
        auto& cell = getCell(coord);

        if (!cell.hasEntity() || !cell.entity->isShell()) {
            continue; // Bad tracking of shells
        }
        auto& shell = *entityCast<ObservedShell>(cell.entity.get());
        if(!shell.directionKnown() || shell.getFiredLastTurn()){
            shell.setAsOld();
            continue;
        }

        Coordinates newPos = coord;
        bool destroyed = false;
        for (int step = 0; step < 2; ++step) {
            newPos = DirectionUtils::nextCoordinate(shell.getDirection().value(), newPos, rows, cols);
            auto& nextCell = getCell(newPos);
            if (nextCell.hasEntity() && nextCell.entity->isWall()) {
                cell.clearEntity();
                shellCoordinates.erase(coord);
                destroyed = true;
                break;
            }
        }
        if (!destroyed) {
            moveEntity(coord, newPos);
        }
    }

}

void FullBattleInfo::roundTick() {
    requestInfoCountdown--;
    moveKnownShells();
    for(const Coordinates& coord : friendlyTanksCoordinates){
        auto& cell = getCell(coord);
        if(!cell.hasEntity())
            throw std::runtime_error("Coord set don't match grid");
        entityCast<ObservedTank>(cell.entity.get())->tickUpdate();
    }
    for(const Coordinates& coord : enemyTanksCoordinates){
        auto& cell = getCell(coord);
        if(!cell.hasEntity())
            throw std::runtime_error("Coord set don't match grid");

        entityCast<ObservedTank>(cell.entity.get())->tickUpdate();
    }
}

bool FullBattleInfo::isFriendlyTankAlive(int index) {
    return std::any_of(friendlyTanksCoordinates.begin(), friendlyTanksCoordinates.end(),
                       [&](const auto& coord) {
                           const auto& cell = getCell(coord);
                           const auto& tank = *entityCast<ObservedTank>(cell.entity.get());
                           return tank.getIndex() == index;
                       });
}

void FullBattleInfo::assumeEnemyShellsDirections(int distanceFromTank) {
    const auto& directions = DirectionUtils::getDirections();
    for(const auto& coord : shellCoordinates){
        auto& shell = *entityCast<ObservedShell>(getCell(coord).entity.get());
        if(shell.directionKnown())
            continue;
        for(auto [dy, dx] : directions){
            Coordinates nextPos = coord;
            bool blocked = false;
            Direction dir = DirectionUtils::offSetToDirection(dy, dx);
            for (int step = 0; step < distanceFromTank; ++step) {
                nextPos = DirectionUtils::nextCoordinate(dir, nextPos, rows, cols);
                auto& nextCell = getCell(nextPos);
                if (nextCell.hasEntity() && nextCell.entity->isWall()) {
                    blocked = true;
                    break;
                }
            }
            if(!blocked && enemyTanksCoordinates.count(nextPos)){
                shell.setDirection(DirectionUtils::getOppositeDirection(dir));
                break;
            }
        }

    }
}


