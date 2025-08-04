
#include <sstream>
#include "GameMap.h"
#include <algorithm>
#include "GameEntityUtils.h"

namespace GameManager_206038929_314620071 {
    using namespace EntityUtils;


//Checks the collisions and resolves them for a set of cells.
    void GameMap::resolveCollisions(const std::unordered_set<Cell *> &dirtyCells) {
        unordered_set<size_t> toDelete;
        for (Cell *cell: dirtyCells) {
            std::vector<size_t> entities(cell->entitySet.begin(), cell->entitySet.end());
            for (size_t i = 0; i < entities.size(); ++i) {
                GameEntity &a = entityManager.getEntity(entities[i]);
                if (!a.isAlive()) continue;

                for (size_t j = i + 1; j < entities.size(); ++j) {
                    GameEntity &b = entityManager.getEntity(entities[j]);
                    if (!b.isAlive()) continue;

                    if (!((a.isShell() && b.isMine()) || (a.isMine() && b.isShell()))) {
                        a.hit();
                        b.hit();
                        if (!a.isAlive()) toDelete.insert(entities[i]);
                        if (!b.isAlive()) toDelete.insert(entities[j]);
                    }
                }
            }
        }
        for (Cell *cell: dirtyCells) {
            for (size_t deadId: toDelete) {
                cell->entitySet.erase(deadId);
            }
        }
        for (size_t entityIndex: toDelete) {
            GameEntity &entity = entityManager.getEntity(entityIndex);
            if (entity.isShell()) shellsIds.erase(entityIndex);
            if (entity.isTank()) {
                tankIds.erase(entityIndex);
            }
            entityManager.destroyEntity(entityIndex);
        }
    }


//Checks if the tanks are about to go past each other, if so destroys them preemptively.
    void GameMap::tanksAboutToCollide() {
        unordered_set<size_t> toDelete;
        for (size_t tankId1: tankIds) {
            auto &tank1 = *entityCast<Tank>(&entityManager.getEntity(tankId1));
            ActionRequest action1 = tank1.peekAction();
            if (action1 != ActionRequest::MoveForward && action1 != ActionRequest::MoveBackward) continue;
            for (size_t tankId2: tankIds) {
                if (tankId1 != tankId2) {
                    auto &tank2 = *entityCast<Tank>(&entityManager.getEntity(tankId2));
                    ActionRequest action2 = tank2.peekAction();
                    if (action2 != ActionRequest::MoveForward && action2 != ActionRequest::MoveBackward) continue;
                    Direction direction1 = action1 == ActionRequest::MoveForward ? tank1.getDirection()
                                                                                 : DirectionUtils::getOppositeDirection(
                                    tank1.getDirection());
                    Direction direction2 = action2 == ActionRequest::MoveForward ? tank2.getDirection()
                                                                                 : DirectionUtils::getOppositeDirection(
                                    tank2.getDirection());
                    Coordinates oldCoords1 = tank1.getCoords();
                    Coordinates oldCoords2 = tank2.getCoords();
                    Coordinates newCoords1 = getNewPosition(tank1, direction1);
                    Coordinates newCoords2 = getNewPosition(tank2, direction2);
                    if (newCoords1 == oldCoords2 && newCoords2 == oldCoords1) {
                        toDelete.insert(tankId1);
                        toDelete.insert(tankId2);
                    }
                }
            }
        }
        for (size_t tankId: toDelete) {
            GameEntity &tank = entityManager.getEntity(tankId);
            getCell(tank.getCoords()).eraseEntity(tankId);
            entityManager.destroyEntity(tankId);
            tankIds.erase(tankId);
        }

    }

//Checks if any two shells are about to go past each other, if so destroys them preemptively.
    void GameMap::shellsAboutToCollide() {
        unordered_set<size_t> toDelete;
        for (size_t shellId1: shellsIds) {
            for (size_t shellId2: shellsIds) {
                if (shellId1 != shellId2) {
                    auto &shell1 = *entityCast<Shell>(&entityManager.getEntity(shellId1));
                    auto &shell2 = *entityCast<Shell>(&entityManager.getEntity(shellId2));
                    Coordinates oldCoords1 = shell1.getCoords();
                    Coordinates oldCoords2 = shell2.getCoords();
                    Direction direction1 = shell1.getDirection();
                    Direction direction2 = shell2.getDirection();
                    Coordinates newCoords1 = getNewPosition(shell1, direction1);
                    Coordinates newCoords2 = getNewPosition(shell2, direction2);

                    if (newCoords1 == oldCoords2 && newCoords2 == oldCoords1) {
                        toDelete.insert(shellId1);
                        toDelete.insert(shellId2);
                    }
                }

            }
        }
        for (size_t shellId: toDelete) {
            GameEntity &shell = entityManager.getEntity(shellId);
            getCell(shell.getCoords()).eraseEntity(shellId);
            entityManager.destroyEntity(shellId);
            shellsIds.erase(shellId);
        }
    }

//Calculates the position of an entity if it were to move in a direction. Does not move the entity.
    Coordinates GameMap::getNewPosition(const GameEntity &entity, Direction dir) const {
        return DirectionUtils::nextCoordinate(dir, entity.getCoords(), rows, cols);
    }

//Moves the entity in given direction
    void GameMap::moveEntity(GameEntity &entity, Direction dir) {
        Coordinates currCoords = entity.getCoords();
        Coordinates newCoords = getNewPosition(entity, dir);
        getCell(newCoords).insertEntity(entity);
        getCell(currCoords).eraseEntity(entity);
        entity.setCoords(newCoords);
    }

//Moves all shells one step in their direction, except shells created this turn. This is because shells already get created 1 cell away from the tank.
    void GameMap::moveShells() {
        for (size_t shellId: shellsIds) {
            auto &shell = *entityCast<Shell>(&entityManager.getEntity(shellId));
            if (shell.isNewShell()) {
                shell.setAsOld();
            } else {
                Direction dir = shell.getDirection();
                moveEntity(shell, dir);
            }
        }
    }

//Fires a shell from given tank, in the direction the tank is facing. Creates the shell one cell away from the tank.
    void GameMap::fireShell(const Tank &tank) {
        Coordinates coords = getNewPosition(tank, tank.getDirection());
        auto y = coords.yAsSizeT();
        auto x = coords.xAsSizeT();
        Cell *cell = &getCell(y, x);
        const GameEntity &shell = entityManager.createShell(y, x, tank.getDirection());
        size_t entityIndex = shell.getEntityId();
        cell->insertEntity(entityIndex);
        shellsIds.insert(entityIndex);
    }

//Public function that checks and resolves the collisions in cells where tanks and cell exist only for efficiency.
    void GameMap::checkCollisions() {
        unordered_set<Cell *> dirtyCells;
        for (size_t tankId: tankIds) {
            Coordinates coords = entityManager.getEntity(tankId).getCoords();
            dirtyCells.insert(&getCell(coords));
        }

        for (size_t shellId: shellsIds) {
            Coordinates coords = entityManager.getEntity(shellId).getCoords();
            dirtyCells.insert(&getCell(coords));
        }
        resolveCollisions(dirtyCells);
    }


//Returns true iff there's no wall in 1 cell away from the tank in the given direction
    bool GameMap::tankCanMoveInDirection(const Tank &tank, Direction dir) const {
        Coordinates newCoords = getNewPosition(tank, dir);
        const Cell &targetCell = getCell(newCoords);
        return !targetCell.hasWall(entityManager);
    }

    std::unique_ptr<ConcreteSatelliteView> GameMap::getSatelliteView() {
        std::vector<std::vector<char>> view(rows, std::vector<char>(cols, ' '));
        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                const Cell &cell = getCell(y, x);
                if (!cell.entitySet.empty()) {
                    size_t entityId = *cell.entitySet.begin();
                    const GameEntity &entity = entityManager.getEntity(entityId);
                    view[y][x] = entity.getSymbol();
                }
            }
        }

        for (size_t shellId: shellsIds) {
            GameEntity &shell = entityManager.getEntity(shellId);
            auto y = shell.getYAsSizeT();
            auto x = shell.getXAsSizeT();
            view[y][x] = shell.getSymbol();
        }
        return std::make_unique<ConcreteSatelliteView>(rows, cols, std::move(view));
    }

    void GameMap::buildMap(const SatelliteView &view, size_t height, size_t width, size_t numShells) {
        rows = height;
        cols = width;
        grid = vector<vector<Cell>>(rows, vector<Cell>(cols));
        int tankCount = 0;
        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                grid[y][x].setCoordinates({y, x});
                handleCell(y, x , view.getObjectAt(x, y), tankCount, numShells);
            }
        }
    }

    void GameMap::handleCell(size_t y, size_t x, char cell, int& tankCount, size_t numShells) {
        if (cell >= '1' && cell <= '2') {
            int playerNumber = cell - '0';
            const Tank &tank = *entityCast<Tank>(
                    &entityManager.createTank(y, x, playerNumber % 2 == 1 ? Direction::Left : Direction::Right,
                                              playerNumber, tankCount, numShells)
            );
            tankCount++;
            grid[y][x].entitySet.insert(tank.getEntityId());
            tankIds.insert(tank.getEntityId());
            tempTankIdsVector.push_back(tank.getEntityId());
            return;
        }
        switch (cell) {
            case ' ':
                return;
            case '#': {
                const GameEntity &entity = entityManager.createWall(y, x);
                grid[y][x].entitySet.insert(entity.getEntityId());
                return;
            }
            case '@': {
                const GameEntity &entity = entityManager.createMine(y, x);
                grid[y][x].entitySet.insert(entity.getEntityId());
                return;
            }
            default:
                handleBadCharacter(y, x);
        }
    }

    void GameMap::handleBadCharacter(size_t y, size_t x) {
        std::cout << "GameManager_206038929_314620071 got an unexpected character at (" << y << ", " << x << "), filling as space.\n";
    }
}



