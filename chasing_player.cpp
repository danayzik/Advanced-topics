#include "players.h"
#include "game_map.h"
#include "binary_heap.h"


Action ChasingPlayer::requestAction(const GameMap &gameMap) {
    int rows = gameMap.getGrid().size();
    int cols = gameMap.getGrid()[0].size();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    BinaryHeap<CellSearchItem> heap;
    vector<vector<bool>> visited(rows, vector<bool>(cols, false));
}