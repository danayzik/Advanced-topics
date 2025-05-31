#include "../include/cell.h"
#include "../include/entities.h"
#include <algorithm>



bool Cell::hasWall(const EntityManager &entityManager) const {
    return std::any_of(entitySet.begin(), entitySet.end(), [&](size_t entityIndex) {
        return entityManager.getEntity(entityIndex).isWall();
    });
}