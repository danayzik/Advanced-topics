#include "FirstTankAlgorithm.h"
#include <queue>
#include <algorithm>

void FirstTankAlgorithm::roundTick() {
    roundCounter++;
    stepsSinceLastCalculation++;
    stepsSinceLastInfoRequest++;
    if(initialized){
        battleInfo.moveKnownShells();

    }
}

void FirstTankAlgorithm::updateBattleInfo(BattleInfo &info) {
    auto& newInfo= dynamic_cast<FullBattleInfo&>(info);
    if(!initialized){
        battleInfo = newInfo;
        initGraph();
    }
    else{
        newInfo.updateFromTankInfo(battleInfo);
        battleInfo = newInfo;
    }
}



ActionRequest FirstTankAlgorithm::requestAction(ActionRequest action) {
    auto& myTank = battleInfo.getMyTank();
    switch (action){ //Never requesting backward
        case ActionRequest::GetBattleInfo:
            stepsSinceLastInfoRequest = 0;
            break;
        case ActionRequest::DoNothing:
            break;
        case ActionRequest::MoveForward:
            battleInfo.moveMyTankForward();
            break;
        case ActionRequest::Shoot:
            myTank.fire();
            battleInfo.fireShellFromTank(myTank);
            break;
        default:
            myTank.rotate(rotateActionToAngle(action));
            break;
    }
    return action;
}


ActionRequest FirstTankAlgorithm::getAction() {
    roundTick();
    if(stepsSinceLastInfoRequest > 3){
        return requestAction(ActionRequest::GetBattleInfo);
    }
    const ObservedTank& myTank = battleInfo.getMyTank();
    const Coordinates* closest = nullptr;
    const auto& enemyTanksCoordinates = battleInfo.getEnemyTanksCoordinates();
    closest = &(*std::min_element(
            enemyTanksCoordinates.begin(),
            enemyTanksCoordinates.end(),
            [&](const Coordinates& a, const Coordinates& b) {

                return distanceBetweenTanks(battleInfo, myTank, a.yCoord, a.xCoord) <
                       distanceBetweenTanks(battleInfo, myTank, b.yCoord, b.xCoord);
            }));

    if(stepsSinceLastCalculation > 4){
        calculateGridPathsFromTarget(*closest);
    }
    int currY = myTank.getY();
    int currX = myTank.getX();
    int dist = distanceBetweenTanks(battleInfo, myTank, closest->yCoord, closest->xCoord);
    if(gridGraph[currY][currX].totalCost == INT32_MAX && !myTank.canFire())
        return requestAction(ActionRequest::GetBattleInfo);

    bool shouldFire = (dist < 3 || gridGraph[currY][currX].totalCost == INT32_MAX) && myTank.canFire() &&
                      enemyInLineOfSight(battleInfo, myTank, closest->yCoord, closest->xCoord);
    if(shouldFire)return requestAction(ActionRequest::Shoot);
    return getBestMovement();
}

void FirstTankAlgorithm::resetGraph() {
    for(size_t i = 0; i < gridGraph.size(); i++){
        for (size_t j = 0; j < gridGraph[i].size(); ++j) {
            gridGraph[i][j].setParams(i, j, INT32_MAX, Up);
        }
    }
}

void FirstTankAlgorithm::initGraph() {
    size_t rows = battleInfo.getRows();
    size_t cols = battleInfo.getCols();
    gridGraph = std::vector<std::vector<HeapNode>>(rows, std::vector<HeapNode>(cols));
    initialized = true;
}

void FirstTankAlgorithm::calculateGridPathsFromTarget(Coordinates targetCoords) {
    resetGraph();
    const auto& grid = battleInfo.getGrid();
    int cols = battleInfo.getCols();
    int rows = battleInfo.getRows();
    int targetX = targetCoords.xCoord;
    int targetY = targetCoords.yCoord;
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
    stepsSinceLastCalculation = 0;
}

ActionRequest FirstTankAlgorithm::getBestMovement() {
    size_t rows = gridGraph.size();
    size_t cols = gridGraph[0].size();
    std::vector<int> costs(8, 0);
    const ObservedTank& myTank = battleInfo.getMyTank();
    Direction currDirection = myTank.getDirection().value();
    Coordinates myCoordinates = battleInfo.getMyTankCoords();
    int currY = myCoordinates.yCoord;
    int currX = myCoordinates.xCoord;
    if(currDirection == gridGraph[currY][currX].optimalDirectionToGo)return requestAction(ActionRequest::MoveForward);
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
        return requestAction(ActionRequest::MoveForward);
    return requestAction(getFirstRotationAction(currDirection, targetDirection));
}