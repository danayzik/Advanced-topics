#pragma once
#include <cmath>
#include "Direction.h"
#include <cstdint>
#include <climits>
#include "ActionRequest.h"
#include "ActionRequestUtils.h"
#include "FullBattleInfo.h"
#include "ObservedEntities.h"
using namespace DirectionUtils;
using namespace ActionRequestUtils;

struct HeapNode{
    int y = 0;
    int x = 0;
    int totalCost = INT32_MAX;
    Direction optimalDirectionToGo = Direction::Up;
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

bool enemyInLineOfSight(const FullBattleInfo &battleInfo, const ObservedTank& myTank, Coordinates targetCoords);
bool hasShellMovingTowardsTank(const FullBattleInfo &battleInfo, const ObservedTank& tank);
std::optional<ActionRequest> rotationTowardsCoords(const ObservedTank& myTank, Coordinates target);
std::optional<ActionRequest> getFirstRotationAction(Direction current, Direction target);
Coordinates getClosestCoordinates(const FullBattleInfo &battleInfo, const std::unordered_set<Coordinates, CoordinatesHash>& coordsSet);
bool friendlyInDirectionWithinRange(const FullBattleInfo &battleInfo, Direction dir, int range);
std::optional<Coordinates> getClosestEnemyInLineOfSight(const FullBattleInfo &battleInfo);
