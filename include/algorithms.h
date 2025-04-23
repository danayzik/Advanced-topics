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

struct Vec2 {
    int y, x;
    inline Vec2(int y_, int x_) : y(y_), x(x_) {}
    inline explicit Vec2(std::pair<int, int> p) : y(p.first), x(p.second) {}
    Vec2 operator-(const Vec2& other) const {
        return {y - other.y, x - other.x};
    }

    bool operator==(const Vec2& other) const {
        return x == other.x && y == other.y;
    }
};





class Algorithm{
public:
    virtual Action getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) = 0;
    virtual ~Algorithm() = default;
    static int distanceBetweenTanks(const GameMap &gameMap, const Tank* tank1, const Tank* tank2);
    static bool enemyInLineOfSight(const GameMap &gameMap, const Tank* enemyTank, const Tank* myTank);
    static bool hasShellMovingTowardsTank(const GameMap &gameMap, const Tank* tank);
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