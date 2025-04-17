
#include <sstream>
#include "../include/game_map.h"
#include "../include/map_loader.h"
#include <algorithm>
#ifdef USE_SFML
#include "../include/sfml_renderer.h"
#endif
#include "../include/noop_renderer.h"


GameMap::GameMap(const string& filePath) {
    rows = 0;
    cols = 0;
    MapLoader::loadMap(filePath, this);

    #ifdef USE_SFML
    renderer = new SFMLRenderer(rows, cols);
    #else
    renderer = new NoOpRenderer();
    #endif

    renderer->initialize();
    renderer->drawGrid(grid);
}

GameResult GameMap::getGameResult() const {
    bool oneEmpty = playerOneTanks.empty();
    bool twoEmpty = playerTwoTanks.empty();
    if(oneEmpty && twoEmpty)
        return Draw;
    if(oneEmpty)
        return PlayerTwoWin;
    if(twoEmpty)
        return PlayerOneWin;
    return NotOver;
}

void GameMap::resolveCollisions(const std::unordered_set<Cell*>& dirtyCells) {
    unordered_set<GameEntity*> toDelete;
    for (Cell* cell : dirtyCells) {
        std::vector<GameEntity*> entities(cell->entitySet.begin(), cell->entitySet.end());
        for (size_t i = 0; i < entities.size(); ++i) {
            GameEntity* a = entities[i];
            if (!a->isAlive()) continue;

            for (size_t j = i + 1; j < entities.size(); ++j) {
                GameEntity* b = entities[j];
                if (!b->isAlive()) continue;

                if (!(a->isShell() && b->isMine() || a->isMine() && b->isShell()) ) {
                    a->hit();
                    b->hit();
                    if (!a->isAlive()) toDelete.insert(a);
                    if (!b->isAlive()) toDelete.insert(b);
                }
            }
        }
    }
    for (GameEntity* e : toDelete) {
        if (e->isShell()) shells.erase(dynamic_cast<Shell*>(e));
        if (e->isTank()){
            playerOneTanks.erase(dynamic_cast<Tank*>(e));
            playerTwoTanks.erase(dynamic_cast<Tank*>(e));
        }
        delete e;
    }
}

void GameMap::updateVisuals() {
    renderer->drawGrid(grid);

}

bool GameMap::tanksAboutToCollide(Tank* tank1,Tank* tank2){
    Action action1 = tank1->peekAction();
    Action action2 = tank2->peekAction();
    if (action1 != MoveForward && action1 != MoveBackward) return false;
    if (action2 != MoveForward && action2 != MoveBackward) return false;
    Direction direction1 = action1 == MoveForward ? tank1->getDirection() : getOppositeDirection(tank1->getDirection());
    Direction direction2 = action2 == MoveForward ? tank2->getDirection() : getOppositeDirection(tank2->getDirection());
    auto [newY1, newX1] = getNewPosition(tank1, direction1);
    auto [newY2, newX2] = getNewPosition(tank2, direction2);
    if ((newY1 == tank2->getY() && newX1 == tank2->getX()) && (newY2 == tank1->getY() && newX2 == tank1->getX())) {
        playerOneTanks.erase(tank1);
        playerTwoTanks.erase(tank2);
        delete tank1;
        delete tank2;
        return true;
    }
    return false;
}

void GameMap::shellsAboutToCollide() {
    unordered_set<Shell*> toDelete;
    for (Shell* shell1 : shells) {
        for (Shell* shell2 : shells) {
            if(shell1 != shell2){
                Direction direction1 = shell1->getDirection();
                Direction direction2 = shell2->getDirection();
                auto [newY1, newX1] = getNewPosition(shell1, direction1);
                auto [newY2, newX2] = getNewPosition(shell2, direction2);
                if ((newY1 == shell2->getY() && newX1 == shell2->getX()) && (newY2 == shell1->getY() && newX2 == shell1->getX())) {
                    toDelete.insert(shell1);
                    toDelete.insert(shell2);
                }
            }

        }
    }
    for (Shell* shell : toDelete) {
        shells.erase(shell);
        delete shell;
    }
}


std::pair<int, int> GameMap::getNewPosition(const GameEntity* entity, Direction dir) const {
    int currX = entity->getX();
    int currY = entity->getY();
    auto [dy, dx] = directionToCoordinatesOffset(dir);
    int newX = (currX + dx + cols) % cols;
    int newY = (currY + dy + rows) % rows;
    return {newY, newX};
}

void GameMap::moveEntity(GameEntity* entity, Direction dir){
    int currX = entity->getX();
    int currY = entity->getY();
    auto [newY, newX] = getNewPosition(entity, dir);
    grid[newY][newX].entitySet.insert(entity);
    grid[currY][currX].entitySet.erase(entity);
    entity->setCoords(newY, newX);
    entity->setCell(&grid[newY][newX]);
}

void GameMap::moveShells(){
    for (Shell* shell : shells) {
        if(shell->isNewShell()){
           shell->setAsOld();
        }
        else{
            Direction dir = shell->getDirection();
            moveEntity(shell, dir);
        }
    }
}

void GameMap::createShell(Tank* tank) {
    auto[y, x] = getNewPosition(tank, tank->getDirection());
    Cell* cell = &grid[y][x];
    auto* shell = new Shell(y, x, cell, tank->getDirection());
    cell->entitySet.insert(shell);
    shells.insert(shell);

}


void GameMap::checkCollisions(){
    unordered_set<Cell*> dirtyCells;
    for (Tank* tank : playerOneTanks) {
        Cell* tankCell = tank->getCell();
        dirtyCells.insert(tankCell);
    }
    for (Tank* tank : playerTwoTanks) {
        Cell* tankCell = tank->getCell();
        dirtyCells.insert(tankCell);
    }
    for (Shell* shell : shells) {
        Cell* shellCell = shell->getCell();
        dirtyCells.insert(shellCell);
    }
    resolveCollisions(dirtyCells);
}



bool GameMap::tankCanMoveInDirection(const Tank* tank, Direction dir) const {
    auto [newY, newX] = getNewPosition(tank, dir);
    const Cell& targetCell = grid[newY][newX];
    return std::all_of(
            targetCell.entitySet.begin(),
            targetCell.entitySet.end(),
            [](GameEntity* entity) {
                return !entity->isWall();
            }
    );
}

const Tank *GameMap::getEnemyTank(int playerNumber) const {
    if(playerNumber == 1){
        return *playerTwoTanks.begin();
    }
    if(playerNumber == 2){
        return *playerOneTanks.begin();
    }
    return nullptr;
}



