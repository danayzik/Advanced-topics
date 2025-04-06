#include <cmath>
#include <sstream>
#include "game_map.h"
#include "map_loader.h"
#include "entities.h"




GameMap::GameMap(const string& filePath) {
    MapData mapData = MapLoader::loadMap(filePath);
    rows = mapData.rows;
    cols = mapData.cols;
    grid = mapData.grid;
    tanks.insert(mapData.tank1);
    tanks.insert(mapData.tank2);

}
std::pair<int, int> GameMap::getNewPosition(const GameEntity* entity, Direction dir) const {
    int currX = entity->getX();
    int currY = entity->getY();
    double radians = dir * PI / 180.0;
    int dx = static_cast<int>(round(cos(radians)));
    int dy = static_cast<int>(round(sin(radians)));
    int newX = (currX + dx + cols) % cols;
    int newY = (currY + dy + rows) % rows;
    return {newY, newX};
}

void GameMap::moveEntity(GameEntity* entity, Direction dir){
    Cell* currCell = entity->getCell();
    int currX = entity->getX();
    int currY = entity->getY();
    auto [newY, newX] = getNewPosition(entity, dir);
    grid[newY][newX].entitySet.insert(entity);
    grid[currY][currX].entitySet.erase(entity);
    entity->setCoords(newX, newY);
    entity->setCell(&grid[newY][newX]);
}

void GameMap::moveShellsAndCheckCollisions(){
    unordered_set<Shell*> shellsToRemove;
    unordered_set<Tank*> tanksToRemove;
    for (Shell* shell : shells) {
        Direction dir = shell->getDirection();
        moveEntity(shell, dir);
    }
    //Add shells going past each other //Custom destructor that removes the shells from the game?
    for (Shell* shell : shells) {
        Cell* cellShell = shell->getCell();
        for(GameEntity* entity : cellShell->entitySet){
            if(entity != shell){
                if(entity->collide()){
                    shellsToRemove.insert(shell);
                }
            }
        }
    }
    for(Shell* shell : shellsToRemove){
        shell->getCell()->entitySet.erase(shell);
        shells.erase(shell);
        delete shell;
    }
}

bool GameMap::tankCanMoveInDirection(const Tank* tank, Direction dir) const {
    auto [newY, newX] = getNewPosition(tank, dir);
    const Cell& targetCell = grid[newY][newX];
    for(GameEntity* entity: targetCell.entitySet){
        if(entity->isWall())
            return false;
    }
    return true;

}



