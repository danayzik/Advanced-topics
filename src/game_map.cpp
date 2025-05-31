
#include <sstream>
#include "../include/game_map.h"
#include "../include/map_loader.h"
#include <algorithm>
#ifdef USE_SFML
#include "../include/sfml_renderer.h"
#endif
#include "../include/noop_renderer.h"

//This file includes pre-processor commands based on build options.
//Map is loaded by the Map loader.
void GameMap::readBoard(const std::string &mapFilePath, GameManager &gameManager) {
    MapLoader mapLoader{mapFilePath, *this, gameManager};
    mapLoader.loadMap();
    #ifdef USE_SFML
        renderer = std::make_unique<SFMLRenderer>(rows, cols);
    #else
        renderer = std::make_unique<NoOpRenderer>();
    #endif
    renderer->initialize();
    renderer->drawGrid(grid);
}




//Checks the collisions and resolves them for a set of cells.
void GameMap::resolveCollisions(const std::unordered_set<Cell*>& dirtyCells) {
    unordered_set<size_t> toDelete;
    for (Cell* cell : dirtyCells) {
        std::vector<size_t > entities(cell->entitySet.begin(), cell->entitySet.end());
        for (size_t i = 0; i < entities.size(); ++i) {
            GameEntity& a = entityManager.getEntity(entities[i]);
            if (!a.isAlive()) continue;

            for (size_t j = i + 1; j < entities.size(); ++j) {
                GameEntity& b = entityManager.getEntity(entities[j]);
                if (!b.isAlive()) continue;

                if (!((a.isShell() && b.isMine()) || (a.isMine() && b.isShell())) ) {
                    a.hit();
                    b.hit();
                    if (!a.isAlive()) toDelete.insert(entities[i]);
                    if (!b.isAlive()) toDelete.insert(entities[j]);
                }
            }
        }
    }
    for (Cell* cell : dirtyCells) {
        for (size_t deadId : toDelete) {
            cell->entitySet.erase(deadId);
        }
    }
    for (size_t entityIndex : toDelete) {
        GameEntity& entity = entityManager.getEntity(entityIndex);
        if (entity.isShell()) shellsIds.erase(entityIndex);
        if (entity.isTank()){
            tankIds.erase(entityIndex);
        }
        entityManager.destroyEntity(entityIndex);
    }
}

void GameMap::updateVisuals() {
    renderer->drawGrid(grid);
}

//Checks if the tanks are about to go past each other, if so destroys them preemptively.
void GameMap::tanksAboutToCollide(){
    unordered_set<size_t > toDelete;
    for (size_t tankId1 : tankIds) {
        auto& tank1 = dynamic_cast<Tank&>(entityManager.getEntity(tankId1));
        ActionRequest action1 = tank1.peekAction();
        if (action1 != ActionRequest::MoveForward && action1 != ActionRequest::MoveBackward) continue;
        for (size_t tankId2 : tankIds) {
            if(tankId1 != tankId2){
                auto& tank2 = dynamic_cast<Tank&>(entityManager.getEntity(tankId2));
                ActionRequest action2 = tank2.peekAction();
                if (action2 != ActionRequest::MoveForward && action2 != ActionRequest::MoveBackward) continue;
                Direction direction1 = action1 == ActionRequest::MoveForward ? tank1.getDirection() : getOppositeDirection(tank1.getDirection());
                Direction direction2 = action2 == ActionRequest::MoveForward ? tank2.getDirection() : getOppositeDirection(tank2.getDirection());
                auto [newY1, newX1] = getNewPosition(tank1, direction1);
                auto [newY2, newX2] = getNewPosition(tank2, direction2);
                if ((newY1 == tank2.getY() && newX1 == tank2.getX()) && (newY2 == tank1.getY() && newX2 == tank1.getX())) {
                    toDelete.insert(tankId1);
                    toDelete.insert(tankId2);
                }
            }
        }
    }
    for (size_t tankId : toDelete) {
        entityManager.destroyEntity(tankId);
        tankIds.erase(tankId);
    }

}

//Checks if any two shells are about to go past each other, if so destroys them preemptively.
void GameMap::shellsAboutToCollide() {
    unordered_set<size_t > toDelete;
    for (size_t shellId1 : shellsIds) {
        for (size_t shellId2 : shellsIds) {
            if(shellId1 != shellId2){
                auto& shell1 = dynamic_cast<Shell&>(entityManager.getEntity(shellId1));
                auto& shell2 = dynamic_cast<Shell&>(entityManager.getEntity(shellId2));
                Direction direction1 = shell1.getDirection();
                Direction direction2 = shell2.getDirection();
                auto [newY1, newX1] = getNewPosition(shell1, direction1);
                auto [newY2, newX2] = getNewPosition(shell2, direction2);
                if ((newY1 == shell2.getY() && newX1 == shell2.getX()) && (newY2 == shell1.getY() && newX2 == shell1.getX())) {
                    toDelete.insert(shellId1);
                    toDelete.insert(shellId2);
                }
            }

        }
    }
    for (size_t shellId : toDelete) {
        entityManager.destroyEntity(shellId);
        shellsIds.erase(shellId);
    }
}

//Calculates the position of an entity if it were to move in a direction. Does not move the entity.
std::pair<size_t, size_t> GameMap::getNewPosition(const GameEntity& entity, Direction dir) const {
    auto [currY, currX] = entity.getCoords();
    auto [dy, dx] = directionToCoordinatesOffset(dir);
    auto newX = static_cast<size_t>((static_cast<int>(currX) + dx + static_cast<int>(cols)) % static_cast<int>(cols));
    auto newY = static_cast<size_t>((static_cast<int>(currY) + dy + static_cast<int>(rows)) % static_cast<int>(rows));
    return {newY, newX};
}

//Moves the entity in given direction
void GameMap::moveEntity(GameEntity& entity, Direction dir){
    auto [currY, currX] = entity.getCoords();
    auto [newY, newX] = getNewPosition(entity, dir);
    grid[newY][newX].entitySet.insert(entity.getEntityId());
    grid[currY][currX].entitySet.erase(entity.getEntityId());
    entity.setCoords(newY, newX);
}

//Moves all shells one step in their direction, except shells created this turn. This is because shells already get created 1 cell away from the tank.
void GameMap::moveShells(){
    for (size_t shellId : shellsIds) {
        auto& shell = dynamic_cast<Shell&>(entityManager.getEntity(shellId));
        if(shell.isNewShell()){
           shell.setAsOld();
        }
        else{
            Direction dir = shell.getDirection();
            moveEntity(shell, dir);
        }
    }
}

//Fires a shell from given tank, in the direction the tank is facing. Creates the shell one cell away from the tank.
void GameMap::fireShell(const Tank& tank) {
    auto[y, x] = getNewPosition(tank, tank.getDirection());
    Cell* cell = &grid[y][x];
    size_t entityIndex = entityManager.createShell(y, x, tank.getDirection());
    cell->entitySet.insert(entityIndex);
    shellsIds.insert(entityIndex);
}

//Public function that checks and resolves the collisions in cells where tanks and cell exist only for efficiency.
void GameMap::checkCollisions(){
    unordered_set<Cell*> dirtyCells;
    for(size_t tankId : tankIds){
        auto [y, x] = entityManager.getEntity(tankId).getCoords();
        dirtyCells.insert(&grid[y][x]);
    }

    for (size_t shellId : shellsIds) {
        auto [y, x] = entityManager.getEntity(shellId).getCoords();
        dirtyCells.insert(&grid[y][x]);
    }
    resolveCollisions(dirtyCells);
}


//Returns true iff there's no wall in 1 cell away from the tank in the given direction
bool GameMap::tankCanMoveInDirection(const Tank& tank, Direction dir) const {
    auto [newY, newX] = getNewPosition(tank, dir);
    const Cell& targetCell = grid[newY][newX];
    return targetCell.hasWall(entityManager);
}

std::unique_ptr<SatelliteView> GameMap::getSatelliteView(const Tank& tank) {
    std::vector<std::vector<char>> view(rows, std::vector<char>(cols, '.'));
    for (size_t y = 0; y < rows; ++y) {
        for (size_t x = 0; x < cols; ++x) {
            const Cell& cell = grid[y][x];
            if (!cell.entitySet.empty()) {
                size_t entityId = *cell.entitySet.begin();
                const GameEntity& entity = entityManager.getEntity(entityId);
                view[y][x] = entity.getSymbol();
            }
        }
    }

    for(size_t shellId : shellsIds){
        GameEntity& shell = entityManager.getEntity(shellId);
        auto [y, x] = shell.getCoords();
        view[y][x] = shell.getSymbol();
    }
    auto [tankY, tankX] = tank.getCoords();
    view[tankY][tankX] = '%';
    return std::make_unique<ConcreteSatelliteView>(rows, cols, std::move(view));
}






