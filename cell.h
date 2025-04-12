#pragma once
#include <unordered_set>

class GameEntity;

using std::unordered_set;

struct Cell {
    unordered_set<GameEntity*> entitySet;
    bool isPassableForTank() const;
    bool isPassableForShell() const;
    unsigned int y;
    unsigned int x;
};
