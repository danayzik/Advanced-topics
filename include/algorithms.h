#pragma once
#include "actions.h"
#include "game_map.h"


struct HeapNode{
    int y = 0;
    int x = 0;
    int totalCost = INT32_MAX;
    Direction optimalDirectionToGo = Up;
    bool operator>(const HeapNode& other) const {
        return totalCost > other.totalCost;
    }
    bool operator==(const HeapNode& other) const {
        return x == other.x && y == other.y;
    }
    bool operator<(const HeapNode& other) const {
        return totalCost < other.totalCost;
    }
    void setParams(int newY, int newX, int cost, Direction dir){
        y = newY;
        x = newX;
        totalCost = cost;
        optimalDirectionToGo = dir;
    }
};

inline int distanceBetweenTanks(const GameMap &gameMap, const Tank* tank1, const Tank* tank2){
    int rows = gameMap.getRows();
    int cols = gameMap.getCols();
    int firstY = tank1->getY();
    int firstX = tank1->getX();
    int secondY = tank2->getY();
    int secondX = tank2->getX();
    int dy = std::min(abs(secondY - firstY), rows - abs(secondY - firstY));
    int dx = std::min(std::abs(secondX - firstX), cols - std::abs(secondX - firstX));
    return std::max(dy, dx);
}



class Algorithm{
public:
    virtual Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) = 0;
    virtual ~Algorithm() = default;
};


class ChasingAlgorithm : public Algorithm{

private:
    void calculateGridPathsFromTarget(const GameMap& gameMap, int playerNumber);
    vector<vector<HeapNode>> gridGraph;
    int stepsSinceLastCalculation = INT32_MAX;
    bool initialized = false;
    void initGraph(const GameMap& gameMap);
    [[nodiscard]] Action getBestMovement(int currY, int currX, Direction currDirection) const;
    void resetGraph();
    vector<std::pair<int, int>> directions = {
            {0, 1},
            {1, 1},
            {1, 0},
            {1, -1},
            {0, -1},
            {-1, -1},
            {-1, 0},
            {-1, 1}
    };

public:
    Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) override;
};

class AlgorithmV1 : public Algorithm{
private:
    bool enemyInLineOfSight(const GameMap &gameMap, const Tank* enemyTank, const Tank* myTank) const;

public:
    Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) override;
};