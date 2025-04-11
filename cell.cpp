#include "cell.h"
#include "entities.h"
#include <algorithm>

bool Cell::isPassableForTank() const {
    return std::all_of(entitySet.begin(), entitySet.end(),
                       [](GameEntity* e) {
                           return !e->isMine() && !e->isWall();
                       });
}

bool Cell::isPassableForShell() const {
    return std::all_of(entitySet.begin(), entitySet.end(),
                       [](GameEntity* e) {
                           return !e->isWall();
                       });
}