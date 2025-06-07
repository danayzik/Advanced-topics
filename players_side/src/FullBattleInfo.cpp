#include "FullBattleInfo.h"

FullBattleInfo::FullBattleInfo() : rows(0), cols(0), startingShells(0), observedCells() {}

FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells) : rows(rows), cols(cols), startingShells(startingShells), observedCells(rows, std::vector<ObservedCell>(cols))  {}

FullBattleInfo& FullBattleInfo::operator=(const FullBattleInfo& other) {
    if (this == &other) return *this;
    BattleInfo::operator=(other);
    rows = other.rows;
    cols = other.cols;
    startingShells = other.startingShells;
    observedCells = other.observedCells;
    myTankCoords = other.myTankCoords;
    shellCoordinates = other.shellCoordinates;
    friendlyTanksCoordinates = other.friendlyTanksCoordinates;
    enemyTanksCoordinates = other.enemyTanksCoordinates;
    return *this;
}

FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView &satelliteView,
                               int playerIndex) : rows(rows), cols(cols), startingShells(startingShells){
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
                    observedCells[y][x].setEntity(std::make_unique<ObservedTank>(y, x, playerIndex, startingShells, myTankDirection));
                    myTankCoords = {y, x};
                    friendlyTanksCoordinates.insert({y, x});
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
    auto& earlierCells = earlierInfo.getGrid();
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            auto& oldCell = earlierCells[y][x];
            auto& newCell = observedCells[y][x];
            bool bothHaveEntities = oldCell.hasEntity() && newCell.hasEntity();
            if(oldCell.hasMine && newCell.entity->getType() == EntityType::Shell){
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
    Coordinates newCoords = coords.moved(dir, rows, cols);
    auto& cell = observedCells[newCoords.yCoord][newCoords.xCoord];

    if(cell.hasEntity() && cell.entity->isWall())
        return;
    newCoords = newCoords.moved(dir, rows, cols);
    auto& nextCell = observedCells[newCoords.yCoord][newCoords.xCoord];
    if(nextCell.hasEntity() && nextCell.entity->isWall())
        return;
    cell.setEntity(std::make_unique<ObservedShell>(newCoords.yCoord, newCoords.xCoord, dir));
    shellCoordinates.insert(newCoords);
}

void FullBattleInfo::updateFromTankInfo(FullBattleInfo &tankInfo) {
    auto tankCoords = tankInfo.getMyTankCoords();
    auto& tankInfoGrid = tankInfo.getGrid();
    auto& tankCell = observedCells[tankCoords.yCoord][tankCoords.xCoord];
    if(tankCell.hasEntity() && tankCell.entity->getType() == EntityType::Tank){
        tankCell.setEntity(std::move(tankInfoGrid[tankCoords.yCoord][tankCoords.xCoord].entity));
    }
    else{
        throw std::runtime_error("Didn't follow your tank correctly");
    }
    auto& shellCoordsFromTankInfo = tankInfo.getShellsCoordinates();
    for(auto coord : shellCoordsFromTankInfo){
        auto& shellPotentialCell = observedCells[coord.yCoord][coord.xCoord];
        if(shellPotentialCell.hasEntity() && shellPotentialCell.entity->isShell()){
            auto& shell = dynamic_cast<ObservedShell&>(*(shellPotentialCell.entity));
            if(!shell.directionKnown()){
                shell.setDirection((dynamic_cast<ObservedShell&>(*(tankInfoGrid[coord.yCoord][coord.xCoord].entity)).getDirection().value()));
            }
        }
    }

}


void FullBattleInfo::moveMyTankForward() {
    auto& myTank = getMyTank();
    size_t oldY = myTank.getY();
    size_t oldX = myTank.getX();
    myTankCoords = myTankCoords.moved(myTank.getDirection().value(), rows, cols);
    myTank.setCoords(myTankCoords.yCoord, myTankCoords.xCoord);
    observedCells[myTankCoords.yCoord][myTankCoords.xCoord].setEntity(std::move(observedCells[oldY][oldX].entity));
}

void FullBattleInfo::moveKnownShells() {
    std::unordered_set<Coordinates, CoordinatesHash> updatedCoords;
    for (auto it = shellCoordinates.begin(); it != shellCoordinates.end(); ) {
        Coordinates pos = *it;
        auto& cell = observedCells[pos.yCoord][pos.xCoord];

        if (!cell.hasEntity() || !cell.entity->isShell()) {
            it = shellCoordinates.erase(it);
            continue;
        }


        auto& shell = dynamic_cast<ObservedShell&>(*(cell.entity));
        if(!shell.directionKnown()){
            ++it;
            continue;
        }

        Coordinates newPos = pos;
        bool destroyed = false;
        for (int step = 0; step < 2; ++step) {
            newPos = newPos.moved(shell.getDirection().value(), rows, cols);
            auto& nextCell = observedCells[newPos.yCoord][newPos.xCoord];
            if (nextCell.hasEntity() && nextCell.entity->isWall()) {
                it = shellCoordinates.erase(it);
                cell.clearEntity();
                destroyed = true;
                break;
            }
        }
        if (!destroyed) {
            shell.setCoords(newPos.yCoord, newPos.xCoord);
            observedCells[newPos.yCoord][newPos.xCoord].setEntity(std::move(cell.entity));
            updatedCoords.insert(newPos);
            it = shellCoordinates.erase(it);
        }
    }

    for (const auto& c : updatedCoords) {
        shellCoordinates.insert(c);
    }
}

FullBattleInfo::FullBattleInfo(const FullBattleInfo &other): rows(other.rows), cols(other.cols){
        observedCells.resize(rows);
        for (size_t i = 0; i < rows; ++i) {
            observedCells[i].resize(cols);
            for (size_t j = 0; j < cols; ++j) {
                observedCells[i][j] = other.observedCells[i][j];
            }
        }
    }