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





class Algorithm{
public:
    virtual Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) = 0;
    virtual ~Algorithm() = default;
    static int distanceBetweenTanks(const GameMap &gameMap, const Tank* tank1, const Tank* tank2);
    static bool enemyInLineOfSight(const GameMap &gameMap, const Tank* enemyTank, const Tank* myTank);
    static Action rotationTowardsEnemy(const Tank* enemyTank, const Tank* myTank);
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

public:
    Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) override;
};