#pragma once
#include <unordered_set>
#include "entity_manager.h"
class GameEntity;

using std::unordered_set;

struct Cell {
    unordered_set<size_t> entitySet;
    [[nodiscard]] bool hasWall(const EntityManager& entityManager) const;

    size_t y;
    size_t x;
};
