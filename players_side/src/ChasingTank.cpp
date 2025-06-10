#include "ChasingTank.h"
#include <queue>
#include "algorithm"

ChasingTank::ChasingTank(const FullBattleInfo &battleInfo) : gridGraph(std::vector<std::vector<HeapNode>>(battleInfo.getRows(), std::vector<HeapNode>(battleInfo.getCols()))) {}




void ChasingTank::update(const FullBattleInfo &battleInfo) {
    closestTarget = getClosestCoordinates(battleInfo, battleInfo.getEnemyTanksCoordinates());
    calculateGridPathsFromTarget(battleInfo);
}

ActionRequest ChasingTank::getAction(const FullBattleInfo& battleInfo) {
    const ObservedTank& myTank = battleInfo.getMyTank();
    size_t rows = battleInfo.getRows();
    size_t cols = battleInfo.getCols();
    Coordinates myTankCoords = battleInfo.getMyTankCoords();
    Coordinates closest = getClosestCoordinates(battleInfo, battleInfo.getEnemyTanksCoordinates());
    size_t currY = myTank.getYAsSizeT();
    size_t currX = myTank.getXAsSizeT();
    int dist = myTankCoords.distanceToOtherCoord(closest, rows, cols);
    if(gridGraph[currY][currX].totalCost == INT32_MAX && !myTank.canFire())
        return ActionRequest::GetBattleInfo;
    bool friendlyInRange = friendlyInDirectionWithinRange(battleInfo, myTank.getDirection().value(), dist);
    bool shouldFire = gridGraph[currY][currX].totalCost == INT32_MAX || (!friendlyInRange && dist < 4 && myTank.canFire() &&
                      enemyInLineOfSight(battleInfo, myTank, closest));
    if(shouldFire)return ActionRequest::Shoot;
    std::optional<ActionRequest> advancingActionOpt = getAdvancingToTargetAction(battleInfo);
    if(!advancingActionOpt.has_value())
        return ActionRequest::GetBattleInfo;
    return advancingActionOpt.value();
}



void ChasingTank::calculateGridPathsFromTarget(const FullBattleInfo& battleInfo) {
    resetGraph();
    const auto& grid = battleInfo.getGrid();
    int cols = static_cast<int>(battleInfo.getCols());
    int rows = static_cast<int>(battleInfo.getRows());
    int targetX =  closestTarget.x;
    int targetY =  closestTarget.y;
    std::priority_queue<HeapNode> pq;
    gridGraph[targetY][targetX].setParams(targetY, targetX, 0, Direction::Up);
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
}

void ChasingTank::resetGraph() {
    for(size_t i = 0; i < gridGraph.size(); i++){
        for (size_t j = 0; j < gridGraph[i].size(); ++j) {
            gridGraph[i][j].setParams(static_cast<int>(i), static_cast<int>(j), INT32_MAX, Direction::Up);
        }
    }
}

std::optional<ActionRequest> ChasingTank::getAdvancingToTargetAction(const FullBattleInfo& battleInfo) const {
    int rows = static_cast<int>(gridGraph.size());
    int cols = static_cast<int>(gridGraph[0].size());
    std::vector<int> costs(8, 0);
    const ObservedTank& myTank = battleInfo.getMyTank();
    Direction currDirection = myTank.getDirection().value();
    Coordinates myCoordinates = battleInfo.getMyTankCoords();
    int currY = myCoordinates.y;
    int currX = myCoordinates.x;
    if(currDirection == gridGraph[myCoordinates.yAsSizeT()][myCoordinates.xAsSizeT()].optimalDirectionToGo)return ActionRequest::MoveForward;
    for (size_t i = 0; i < directions.size(); i++) {
        auto [dy, dx] = directions[i];
        int newY = (currY + dy + rows) % rows;
        int newX = (currX + dx + cols) % cols;
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
        return ActionRequest::MoveForward;
    return getFirstRotationAction(currDirection, targetDirection);
}

