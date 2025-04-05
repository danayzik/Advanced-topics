#pragma once
#include <unordered_set>

class GameEntity;

using std::unordered_set;

struct Cell {
    unordered_set<GameEntity*> entitySet;
    inline bool isEmpty() const{
        return entitySet.empty();
    }
};
