#pragma once
#include <unordered_set>

class GameEntity;

using std::unordered_set;

struct Cell {
    unordered_set<GameEntity*> entitySet;
    [[nodiscard]] bool isPassableForTank() const;
    [[nodiscard]] bool isPassableForShell() const;
    unsigned int y;
    unsigned int x;
};
