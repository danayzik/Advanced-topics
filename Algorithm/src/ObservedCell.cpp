#include "ObservedCell.h"
#include <algorithm>

namespace Algorithm_206038929_314620071{

    ObservedCell::ObservedCell(const ObservedCell &other) : mine(other.mine) {
        for(const auto& entity : other.entities){
            entities.push_back(entity->clone());
        }
    }

    void ObservedCell::removeEntity(ObservedEntity *ptrToRemove) {
        auto it = std::find_if(entities.begin(), entities.end(),
                               [ptrToRemove](const std::unique_ptr<ObservedEntity>& uptr) {
                                   return uptr.get() == ptrToRemove;
                               });
        if (it != entities.end())
            entities.erase(it);
    }

    ObservedCell &ObservedCell::operator=(const ObservedCell &other) {
        if (this != &other) {
            mine = other.mine;
            entities.clear();
            for(const auto& entity : other.entities){
                entities.push_back(entity->clone());
            }
        }
        return *this;
    }

    bool ObservedCell::isPassableForShell() const {
        return !std::any_of(entities.begin(), entities.end(), [](const auto& e){ return e->isWall(); });
    }

    bool ObservedCell::isPassableForTank() const {
        if (mine)
            return false;
        return !std::any_of(entities.begin(), entities.end(), [](const auto& e){ return e->isWall(); });
    }

    bool ObservedCell::hasShell() const {
        return std::any_of(entities.begin(), entities.end(), [](const auto& e){ return e->isShell(); });
    }

    std::optional<size_t> ObservedCell::findShellIndex() {
        for (size_t i = 0; i < entities.size(); ++i) {
            if (entities[i]->isShell()) {
                return i;
            }
        }
        return std::nullopt;
    }

    std::optional<size_t> ObservedCell::indexOf(ObservedEntity *ptr) {
        auto it = std::find_if(entities.begin(), entities.end(),
                               [ptr](const std::unique_ptr<ObservedEntity>& uptr) {
                                   return uptr.get() == ptr;
                               });

        if (it != entities.end()) {
            return std::distance(entities.begin(), it);
        } else {
            return std::nullopt;
        }
    }
}
