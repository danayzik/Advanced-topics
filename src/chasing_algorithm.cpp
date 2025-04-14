#include "../include/algorithms.h"
#include <queue>


void ChasingAlgorithm::initGraph(const GameMap& gameMap) {
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    gridGraph = vector<vector<HeapNode>>(rows, vector<HeapNode>(cols));
    initialized = true;

}


void ChasingAlgorithm::resetGraph() {
    for(int i = 0; i < gridGraph.size(); i++){
        for (int j = 0; j < gridGraph[i].size(); ++j) {
            gridGraph[i][j].setParams(i, j, INT32_MAX, Up);
        }
    }
}

void ChasingAlgorithm::calculateGridPathsFromTarget(const GameMap& gameMap, int playerNumber){
    resetGraph();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    const auto& grid = gameMap.getGrid();
    int cols = gameMap.getCols();
    int rows = gameMap.getRows();
    int targetX = enemyTank->getX();
    int targetY = enemyTank->getY();

    std::priority_queue<HeapNode> pq;


    gridGraph[targetY][targetX].setParams(targetY, targetX, 0, enemyTank->getDirection());
    for(auto [dy, dx] : directions){
        int newY = (targetY + dy + rows) % rows;
        int newX = (targetX + dx + cols) % cols;
        if (!grid[newY][newX].isPassableForTank()) continue;
        Direction oppositeDir = getOppositeDirection(offSetToDirection(dy, dx));
        gridGraph[newY][newX].setParams(newY, newX, 1, oppositeDir);
        pq.push(gridGraph[newY][newX]);
    }

    while (!pq.empty()) {
        HeapNode current = pq.top();
        pq.pop();
        if (current.totalCost > gridGraph[current.y][current.x].totalCost) {
            continue;
        }

        for (auto [dy, dx] : directions) {
            int ny = (current.y + dy + rows) % rows;
            int nx = (current.x + dx + cols) % cols;
            if (!grid[ny][nx].isPassableForTank()) continue;
            HeapNode& neighbor = gridGraph[ny][nx];
            Direction dirToCurrent = getOppositeDirection(offSetToDirection(dy, dx));
            int rotationPenalty = minimalRotationsNeeded(dirToCurrent, current.optimalDirectionToGo);
            int newDist = current.totalCost + 1 + rotationPenalty;
            if (newDist < neighbor.totalCost) {
                neighbor.totalCost = newDist;
                neighbor.optimalDirectionToGo = dirToCurrent;
                pq.push(neighbor);
            }
        }
    }
    stepsSinceLastCalculation = 0;
}


Action ChasingAlgorithm::getBestMovement(int currY, int currX, Direction currDirection) const{
    int rows = gridGraph.size();
    int cols = gridGraph[0].size();
    vector<int> costs(8, 0);
    if(currDirection == gridGraph[currY][currX].optimalDirectionToGo)return MoveForward;
    for (int i = 0; i < directions.size(); i++) {
        auto [dy, dx] = directions[i];
        int newY = (currY + dy + rows) % rows;
        int newX = (currX + dx + cols) % cols;
        const HeapNode& node = gridGraph[newY][newX];
        int cost;
        if(node.totalCost == INT32_MAX){
            cost = INT32_MAX;
        }
        else{
            cost = node.totalCost + 1 + minimalRotationsNeeded(currDirection, node.optimalDirectionToGo);
        }

        costs[i] = cost;
    }
    auto it = std::min_element(costs.begin(), costs.end());
    int minIndex = it - costs.begin();
    auto [offsetY, offsetX] = directions[minIndex];
    Direction targetDirection = offSetToDirection(offsetY, offsetX);
    return getFirstRotationAction(currDirection, targetDirection);


}

Action ChasingAlgorithm::getAction(const GameMap &gameMap, int playerNumber, const Tank* myTank) {
    if(!initialized){
        initGraph(gameMap);
    }
    if(stepsSinceLastCalculation > 4){
        calculateGridPathsFromTarget(gameMap, playerNumber);
    }
    stepsSinceLastCalculation++;
    int currY = myTank->getY();
    int currX = myTank->getX();
    Direction currDirection = myTank->getDirection();
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    int dist = distanceBetweenTanks(gameMap, myTank, enemyTank);
    bool shouldFire = (dist < 3 || gridGraph[currY][currX].totalCost == INT32_MAX) && myTank->canFire();
    if(shouldFire)return Shoot;
    return getBestMovement(currY, currX, currDirection);
}