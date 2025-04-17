#include "../include/algorithms.h"
#include <queue>
#include <algorithm>

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

/*
 * Modified Dijkstra's algorithm.
 * Calculates distances and optimal direction to continue to in order to reach the enemy tank.
 */
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

/*
 * Given the distances and direction from dijkstra's algorithm;
 * We check what is the best move given our cell and rotation.
 * If we are already facing the optimal direction, we move forward.
 * Otherwise, we calculate our surrounding cells distances and cost to rotate to them and take the minimum.
 */
Action ChasingAlgorithm::getBestMovement(int currY, int currX, Direction currDirection) const{
    size_t rows = gridGraph.size();
    size_t cols = gridGraph[0].size();
    vector<int> costs(8, 0);
    if(currDirection == gridGraph[currY][currX].optimalDirectionToGo)return MoveForward;
    for (int i = 0; i < directions.size(); i++) {
        auto [dy, dx] = directions[i];
        size_t newY = (currY + dy + rows) % rows;
        size_t newX = (currX + dx + cols) % cols;
        const HeapNode& node = gridGraph[newY][newX];
        int cost;
        if(node.totalCost == INT32_MAX){
            cost = INT32_MAX;
        }
        else{
            cost = node.totalCost + 1 + minimalRotationsNeeded(offSetToDirection(dy ,dx), node.optimalDirectionToGo);
        }

        costs[i] = cost;
    }
    auto it = std::min_element(costs.begin(), costs.end());
    size_t minIndex = it - costs.begin();
    auto [offsetY, offsetX] = directions[minIndex];
    Direction targetDirection = offSetToDirection(offsetY, offsetX);
    if(currDirection == targetDirection)
        return MoveForward;
    return getFirstRotationAction(currDirection, targetDirection);

}

/*
 * We fire a shell if the enemy is in our LOS and we are at most at distance 3 from him,
 * Otherwise we chase.
 */
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
    bool shouldFire = (dist < 4 || gridGraph[currY][currX].totalCost == INT32_MAX) && myTank->canFire() &&
            enemyInLineOfSight(gameMap, enemyTank, myTank);
    if(shouldFire)return Shoot;
    return getBestMovement(currY, currX, currDirection);
}