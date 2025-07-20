#include "ObservedCell.h"


namespace Algorithm_206038929_314620071{

    ObservedCell::ObservedCell(const ObservedCell &other) : hasMine(other.hasMine) {
        if (other.entity)
            entity = other.entity->clone();
    }

    ObservedCell &ObservedCell::operator=(const ObservedCell &other) {
        if (this != &other) {
            hasMine = other.hasMine;
            entity = other.entity ? other.entity->clone() : nullptr;
        }
        return *this;
    }

    bool ObservedCell::isPassableForShell() const {
        if (!entity)
            return true;
        return entity->isWall();
    }

    bool ObservedCell::isPassableForTank() const {
        if (hasMine)
            return false;
        if (!entity)
            return true;
        return !entity->isWall();
    }
}
