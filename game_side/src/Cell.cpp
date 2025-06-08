#include "Cell.h"
#include <algorithm>
#include "EntityManager.h"


bool Cell::hasWall(const EntityManager &entityManager) const {
    return std::any_of(entitySet.begin(), entitySet.end(), [&](size_t entityIndex) {
        return entityManager.getEntity(entityIndex).isWall();
    });
}