#include "players.h"
#include "game_map.h"
#include <queue>
#include "actions.h"
//Refactor this pile of dogshit and swap to djikstra with rotation penalties
using std::queue;
std::vector<std::pair<int, int>> directions = {
        {0, 1},
        {1, 1},
        {1, 0},
        {1, -1},
        {0, -1},
        {-1, -1},
        {-1, 0},
        {-1, 1}
};

CellSearchItem bfs(const GameMap& gameMap, const Tank* enemyTank, const Tank* yourTank) {
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    int enemyY = enemyTank->getY();
    int enemyX = enemyTank->getX();
    int selfY = yourTank->getY();
    int selfX = yourTank->getX();
    CellSearchItem firstStepCell;
    const auto& grid = gameMap.getGrid();
    queue<CellSearchItem> bfsQueue;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
    bfsQueue.push({enemyY, enemyX, 0});
    visited[enemyY][enemyX] = true;
    bool reached = false;
    while (!bfsQueue.empty() || reached) {
        CellSearchItem current = bfsQueue.front();
        bfsQueue.pop();
        for (const auto& direction : directions) {
            int newY = (current.y + direction.first + rows) % rows;
            int newX = (current.x + direction.second + cols) % cols;
            if (!grid[newY][newX].isPassableForTank()) continue;
            if (!visited[newY][newX]) {
                visited[newY][newX] = true;
                bfsQueue.push({newY, newX, current.distanceFromTarget + 1});
            }
            if(selfY == newY && selfX == newX){
                reached = true;
                firstStepCell = current;
            }
        }
    }
    return firstStepCell;
}

Action ChasingPlayer::requestAction(const GameMap &gameMap) {
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    int selfY = tank->getY();
    int selfX = tank->getX();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    CellSearchItem firstTargetCell = bfs(gameMap, enemyTank, tank);
    int dy = (firstTargetCell.y - selfY + rows) % rows;
    int dx = (firstTargetCell.x - selfX + cols) % cols;
    Direction desiredDirection = offSetToDirection(dy, dx);
    Direction currentFacingDirection = tank->getDirection();
    Action action = getFirstRotationAction(currentFacingDirection, desiredDirection);
    if(action == MoveForward && firstTargetCell.distanceFromTarget < 3 && tank->canFire()){
        action = Shoot;
    }
    return action;
}