#include "cell.h"
#include "entities.h"


bool Cell::isPassable() const{
    for (GameEntity* e : entitySet) {
        if (e->isMine() || e->isWall()) {
            return false;
        }
    }
    return true;
}