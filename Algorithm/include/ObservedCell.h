#pragma once

#include "ObservedEntities.h"

namespace Algorithm_206038929_314620071 {

    class ObservedCell {
    private:

        bool mine = false;
        std::vector<std::unique_ptr<ObservedEntity>> entities;

    public:
        inline void clearMine() { mine = false; }

        inline void setMine() { mine = true; }

        void removeEntity(ObservedEntity* ptrToRemove);

        inline void clearAllEntities(){entities.clear();}

        void addEntity(std::unique_ptr<ObservedEntity> e) {
            entities.push_back(std::move(e));
        }

        inline ObservedEntity* getLastEntityPtr(){
            return entities.empty() ? nullptr : entities.back().get();
        }


        ObservedCell() = default;

        ~ObservedCell() = default;

        ObservedCell(const ObservedCell &other);

        ObservedCell &operator=(const ObservedCell &other);

        ObservedCell(ObservedCell &&) noexcept = default;

        ObservedCell &operator=(ObservedCell &&) noexcept = default;

        std::optional<size_t> findShellIndex();

        [[nodiscard]] std::vector<std::unique_ptr<ObservedEntity>>& getEntities() { return entities;}

        [[nodiscard]] const std::vector<std::unique_ptr<ObservedEntity>>& getEntities() const { return entities;}

        [[nodiscard]] bool isPassableForTank() const;

        [[nodiscard]] bool isPassableForShell() const;

        [[nodiscard]] bool hasShell() const;

        [[nodiscard]] bool hasMine() const{return mine;}

        [[nodiscard]] bool hasEntity() const{return !entities.empty();}

        [[nodiscard]] std::optional<size_t> indexOf(ObservedEntity* ptr);

    };
}