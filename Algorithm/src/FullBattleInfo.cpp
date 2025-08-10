#include "FullBattleInfo.h"
#include <algorithm>
namespace Algorithm_206038929_314620071{

    using namespace EntityUtils;
    using diffEntities = std::vector<unique_ptr<ObservedEntity>>::difference_type;
    FullBattleInfo::FullBattleInfo() : rows(0), cols(0), startingShells(0), observedCells{}, playerIndex(0) {}

    FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, int playerIndex) : rows(rows),
                                                                                                       cols(cols),
                                                                                                       startingShells(
                                                                                                               startingShells),
                                                                                                       observedCells(
                                                                                                               rows,
                                                                                                               std::vector<ObservedCell>(
                                                                                                                       cols)),
                                                                                                       playerIndex(
                                                                                                               playerIndex) {}


    FullBattleInfo::FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView &satelliteView,
                                   int playerIndex) : rows(rows), cols(cols), startingShells(startingShells),
                                                      observedCells(rows, std::vector<ObservedCell>(cols)),
                                                      playerIndex(playerIndex) {
        Direction myTankDirection = playerIndex % 2 == 1 ? Direction::Left : Direction::Right;
        std::unique_ptr<ObservedTank> tankPtr;
        std::unique_ptr<ObservedShell> shellPtr;
        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                char symbol = satelliteView.getObjectAt(x, y);
                ObservedCell& cell = observedCells[y][x];
                if (symbol >= '1' && symbol <= '2') {
                    int playerNumber = symbol - '0';
                    bool friendly = playerNumber == playerIndex;
                    tankPtr = std::make_unique<ObservedTank>(y, x, playerNumber, startingShells,
                                                                  playerNumber % 2 == 1 ? Direction::Left
                                                                                        : Direction::Right);
                    if (friendly)
                        friendlyTanks.push_back(tankPtr.get());
                    else
                        enemyTanks.push_back(tankPtr.get());
                    cell.addEntity(std::move(tankPtr));

                    continue;
                }
                switch (symbol) {
                    case '%':
                        tankPtr = std::make_unique<ObservedTank>(y, x, playerIndex, startingShells,
                                                                      myTankDirection);
                        friendlyTanks.push_back(tankPtr.get());
                        cell.addEntity(std::move(tankPtr));
                        myTank = entityCast<ObservedTank>(cell.getLastEntityPtr());
                        break;
                    case '@':
                        cell.setMine();
                        break;
                    case '#':
                        cell.addEntity(std::make_unique<ObservedWall>(y, x));
                        break;
                    case ' ':
                        break;
                    case '*':
                        shellPtr = std::make_unique<ObservedShell>(y, x);
                        shells.push_back(shellPtr.get());
                        cell.addEntity(std::move(shellPtr));
                        break;
                    default:
                        throw std::runtime_error("Out of bounds or incorrect symbol");
                }
            }
        }
    }


    //Earlier info is invalidated after this update
    //Updates information about shells and mines from previous information
    void FullBattleInfo::updateFromEarlierInfo(FullBattleInfo &earlierInfo) {
        for (auto*& shell : shells){
            Coordinates coords = shell->getCoords();
            auto& oldShellCell = earlierInfo.getCell(coords);
            auto& newShellCell = getCell(coords);
            if (oldShellCell.hasMine())
                newShellCell.setMine();
            auto oldShellIndex = oldShellCell.findShellIndex();
            auto newShellIndex = newShellCell.indexOf(shell);
            assert(newShellIndex);
            if (oldShellIndex) {
                newShellCell.getEntities()[*newShellIndex] = std::move(oldShellCell.getEntities()[*oldShellIndex]);
                auto* newPtr = entityCast<ObservedShell>(newShellCell.getEntities()[*newShellIndex].get());
                shell = newPtr;
                auto& oldEntities = oldShellCell.getEntities();
                oldEntities.erase(oldEntities.begin() + static_cast<diffEntities>(*oldShellIndex));
            }
        }
    }

    void FullBattleInfo::fireShellFromTank(ObservedTank &tank) {
        tank.fire();
        if (!tank.directionKnown())
            return;
        Direction dir = tank.getDirection().value();
        Coordinates coords = tank.getCoords();
        Coordinates newCoords = DirectionUtils::nextCoordinate(dir, coords, rows, cols);
        auto &cell = getCell(newCoords);
        if (!cell.isPassableForShell()) {
            return;
        }
        newCoords = DirectionUtils::nextCoordinate(dir, newCoords, rows, cols);
        auto &nextCell = getCell(newCoords);
        if (!nextCell.isPassableForShell()) {
            return;
        }
        auto ptr = std::make_unique<ObservedShell>(newCoords, dir, true);
        shells.push_back(ptr.get());
        nextCell.addEntity(std::move(ptr));
    }

    //Tank info is invalidated after this
    void FullBattleInfo::updateFromTankInfo(FullBattleInfo &tankInfo) {
        tankIndex = tankInfo.getTankIndex();
        auto* oldMyTank = &tankInfo.getMyTank();
        auto& newCell = getCell(myTank->getCoords());
        oldMyTank->setCoords(myTank->getCoords());
        auto newIndex = newCell.indexOf(myTank);
        auto& oldCell = tankInfo.getCell(oldMyTank->getCoords());
        auto oldIndex = oldCell.indexOf(oldMyTank);
        assert(oldIndex);
        assert(newIndex);
        newCell.getEntities()[*newIndex] = std::move(oldCell.getEntities()[*oldIndex]);
        auto it = std::find(friendlyTanks.begin(), friendlyTanks.end(), myTank);
        if (it != friendlyTanks.end())
            friendlyTanks.erase(it);
        friendlyTanks.push_back(oldMyTank);
        myTank = oldMyTank;
        auto& oldEntities = oldCell.getEntities();
        oldEntities.erase(oldEntities.begin() + static_cast<diffEntities>(*oldIndex));

        updateFromEarlierInfo(tankInfo);
    }


    void FullBattleInfo::moveMyTankForward() {
        Coordinates oldCoords = myTank->getCoords();
        Coordinates newCoords = DirectionUtils::nextCoordinate(myTank->getDirection().value(), myTank->getCoords(), rows, cols);
        auto& oldCell = getCell(oldCoords);
        auto& newCell = getCell(newCoords);
        auto oldIndexOpt = oldCell.indexOf(myTank);
        assert(oldIndexOpt);
        auto& oldEntities = oldCell.getEntities();
        newCell.addEntity(std::move(oldEntities[*oldIndexOpt]));
        oldEntities.erase(oldEntities.begin() + static_cast<diffEntities>(*oldIndexOpt));
        myTank->setCoords(newCoords);
    }

    void FullBattleInfo::setTankIndex(int index) {
        tankIndex = index;
        myTank->setIndex(index);
    }

    void FullBattleInfo::moveKnownShells() {
        std::vector<ObservedShell*> ptrToRemove{};
        for (auto shell: shells) {
            if (!shell->directionKnown() || shell->getFiredLastTurn()) {
                shell->setAsOld();
                continue;
            }
            auto &cell = getCell(shell->getCoords());
            Coordinates newPos = shell->getCoords();
            bool destroyed = false;
            for (int step = 0; step < 2; ++step) {
                newPos = DirectionUtils::nextCoordinate(shell->getDirection().value(), newPos, rows, cols);
                auto &nextCell = getCell(newPos);
                if (!nextCell.isPassableForShell()) {
                    destroyed = true;
                    cell.removeEntity(shell);
                    ptrToRemove.push_back(shell);
                    break;
                }
            }
            if (!destroyed) {
                auto& targetCell = getCell(newPos);
                auto oldIndexOpt = cell.indexOf(shell);
                assert(oldIndexOpt);
                auto& entities = cell.getEntities();
                targetCell.addEntity(std::move(entities[*oldIndexOpt]));
                entities.erase(entities.begin() + static_cast<diffEntities>(*oldIndexOpt));
                shell->setCoords(newPos);
            }
        }
        for (auto* shellPtr : ptrToRemove){
            auto it = std::find(shells.begin(), shells.end(), shellPtr);
            if (it != shells.end())
                shells.erase(it);
        }

    }

    void FullBattleInfo::roundTick() {
        requestInfoCountdown--;
        moveKnownShells();
        myTank->tickUpdate();
    }


    void FullBattleInfo::assumeEnemyShellsDirections(int distanceFromTank) {
        const auto &directions = DirectionUtils::getDirections();
        for (auto* shell: shells) {
            if (shell->directionKnown())
                continue;
            Coordinates coord = shell->getCoords();
            for (auto [dy, dx]: directions) {
                Coordinates nextPos = coord;
                bool blocked = false;
                Direction dir = DirectionUtils::offSetToDirection(dy, dx);
                for (int step = 0; step < distanceFromTank; ++step) {
                    nextPos = DirectionUtils::nextCoordinate(dir, nextPos, rows, cols);
                    auto &nextCell = getCell(nextPos);
                    if (!nextCell.isPassableForShell()) {
                        blocked = true;
                        break;
                    }
                }
                bool enemyAtNextPos = std::any_of(enemyTanks.begin(), enemyTanks.end(),
                                                  [&](ObservedTank* t) { return t->getCoords() == nextPos;});
                if (!blocked && enemyAtNextPos) {
                    shell->setDirection(DirectionUtils::getOppositeDirection(dir));
                    break;
                }
            }
        }
    }

    std::unordered_set<Coordinates, CoordinatesHash> FullBattleInfo::getEnemyTanksCoordinates() const{
        std::unordered_set<Coordinates, CoordinatesHash> enemyCoords{};
        for (auto* enemyTank : enemyTanks){
            enemyCoords.insert(enemyTank->getCoords());
        }
        return enemyCoords;
    }

    void FullBattleInfo::cloneCell(const FullBattleInfo &other, size_t y, size_t x) {
        auto& src = other.getCell(y, x);
        auto& dest = getCell(y, x);
        dest.clearAllEntities();
        if (src.hasMine())
            dest.setMine();
        for (const auto& entity : src.getEntities()) {
            auto ptr = entity->clone();
            switch (ptr->getType()) {
                case EntityType::Shell:
                    shells.push_back(entityCast<ObservedShell>(ptr.get()));
                    break;
                case EntityType::Tank: {
                    auto* tankPtr = entityCast<ObservedTank>(ptr.get());
                    if (playerIndex == tankPtr->getPlayerIndex()) {
                        friendlyTanks.push_back(tankPtr);
                    } else {
                        enemyTanks.push_back(tankPtr);
                    }
                    if (Coordinates{y, x} == other.getMyTankCoords())
                        myTank = tankPtr;
                    break;
                }
                default:
                    break;
            }
            dest.getEntities().push_back(std::move(ptr));
        }
    }

    FullBattleInfo::FullBattleInfo(const FullBattleInfo& other)
            : BattleInfo(other),
              rows(other.rows),
              cols(other.cols),
              startingShells(other.startingShells),
              playerIndex(other.playerIndex),
              tankIndex(other.tankIndex),
              roleToPlay(other.roleToPlay),
              requestInfoCountdown(other.requestInfoCountdown)
    {
        observedCells.resize(rows);
        for (size_t y = 0; y < rows; ++y) {
            observedCells[y].resize(cols);
            for (size_t x = 0; x < cols; ++x) {
                cloneCell(other, y, x);
            }
        }
    }

    FullBattleInfo& FullBattleInfo::operator=(const FullBattleInfo& other) {
        if (this != &other) {

            rows = other.rows;
            cols = other.cols;
            startingShells = other.startingShells;
            playerIndex = other.playerIndex;
            tankIndex = other.tankIndex;
            roleToPlay = other.roleToPlay;
            requestInfoCountdown = other.requestInfoCountdown;
            shells.clear();
            enemyTanks.clear();
            friendlyTanks.clear();
            bool resizeNeeded = observedCells.size() != rows;
            if (resizeNeeded) observedCells.resize(rows);
            for (size_t y = 0; y < rows; ++y) {
                for (size_t x = 0; x < cols; ++x) {
                    if (resizeNeeded) {
                        observedCells[y].resize(cols);
                    }
                    cloneCell(other, y, x);
                }
            }
        }
        return *this;
    }
}
