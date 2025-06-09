#include "FullBattleInfo.h"
#include "utils.h"
using namespace DirectionUtils;
using namespace EntityUtils;
FullBattleInfo::FullBattleInfo() : rows(0), cols(0), startingShells(0), observedCells{} {}

FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells) : rows(rows), cols(cols), startingShells(startingShells), observedCells(rows, std::vector<ObservedCell>(cols))  {}


FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView &satelliteView,
                               int playerIndex) : rows(rows), cols(cols), startingShells(startingShells), observedCells(rows, std::vector<ObservedCell>(cols)){
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
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            auto& oldCell = earlierInfo.getCell(y, x);
            auto& newCell = getCell(y, x);
            bool bothHaveEntities = oldCell.hasEntity() && newCell.hasEntity();
            if(oldCell.hasMine && newCell.hasEntity() && newCell.entity->getType() == EntityType::Shell){
                newCell.setMine();
            }
            if(bothHaveEntities && (oldCell.entity->getType() == newCell.entity->getType())){
                newCell.setEntity(std::move(oldCell.entity));
            }
        }
    }
}


void FullBattleInfo::fireShellFromTank(ObservedTank &tank) {
    if(!tank.directionKnown())
        return;
    Direction dir = tank.getDirection().value();
    Coordinates coords = {tank.getY(), tank.getX()};
    Coordinates newCoords = nextCoordinate(dir, coords, rows, cols);
    auto& cell = getCell(newCoords);

    if(cell.hasEntity() && cell.entity->isWall())
        return;
    newCoords = nextCoordinate(dir, newCoords, rows, cols);
    auto& nextCell = getCell(newCoords);
    if(nextCell.hasEntity() && nextCell.entity->isWall())
        return;
    cell.setEntity(std::make_unique<ObservedShell>(newCoords, dir));
    shellCoordinates.insert(newCoords);
}

void FullBattleInfo::updateFromTankInfo(FullBattleInfo &tankInfo) {
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
            if(!shell.directionKnown()){
                auto* myShell = tankInfo.getCell(coord).entity.get();
                shell.setDirection((entityCast<ObservedShell>(myShell)->getDirection().value()));

            }
        }
    }

}


void FullBattleInfo::moveMyTankForward() {
    auto& myTank = getMyTank();
    Coordinates oldCoords = myTank.getCoords();
    friendlyTanksCoordinates.erase(oldCoords);
    myTankCoords = nextCoordinate(myTank.getDirection().value(), myTankCoords, rows, cols);
    friendlyTanksCoordinates.insert(myTankCoords);
    myTank.setCoords(myTankCoords);
    getCell(myTankCoords).setEntity(std::move(getCell(oldCoords).entity));
}

void FullBattleInfo::moveKnownShells() {
    std::unordered_set<Coordinates, CoordinatesHash> updatedCoords;
    for (auto it = shellCoordinates.begin(); it != shellCoordinates.end(); ) {
        Coordinates pos = *it;
        auto& cell = getCell(pos);

        if (!cell.hasEntity() || !cell.entity->isShell()) {
            it = shellCoordinates.erase(it);
            continue;
        }
        auto& shell = *entityCast<ObservedShell>(cell.entity.get())

        if(!shell.directionKnown()){
            ++it;
            continue;
        }

        Coordinates newPos = pos;
        bool destroyed = false;
        for (int step = 0; step < 2; ++step) {
            newPos = nextCoordinate(shell.getDirection().value(), newPos, rows, cols);
            auto& nextCell = getCell(newPos);
            if (nextCell.hasEntity() && nextCell.entity->isWall()) {
                it = shellCoordinates.erase(it);
                cell.clearEntity();
                destroyed = true;
                break;
            }
        }
        if (!destroyed) {
            shell.setCoords(newPos);
            getCell(newPos).setEntity(std::move(cell.entity));
            updatedCoords.insert(newPos);
            it = shellCoordinates.erase(it);
        }
    }

    for (const auto& c : updatedCoords) {
        shellCoordinates.insert(c);
    }
}

void FullBattleInfo::roundTick() {
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
