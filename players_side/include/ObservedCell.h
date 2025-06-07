#pragma once

#include "ObservedEntities.h"



struct ObservedCell{
    bool hasMine = false;
    std::unique_ptr<ObservedEntity> entity;
    inline void clearMine(){hasMine = false;}
    inline void setMine(){hasMine = true;}
    inline void clearEntity(){entity = nullptr;}
    void setEntity(std::unique_ptr<ObservedEntity> e) {
        entity = std::move(e);
    }

    ObservedCell() = default;
    ~ObservedCell() = default;
    ObservedCell(const ObservedCell& other);
    ObservedCell& operator=(const ObservedCell& other);
    ObservedCell(ObservedCell&&) noexcept = default;
    ObservedCell& operator=(ObservedCell&&) noexcept = default;
    [[nodiscard]] bool isPassableForTank() const;
    [[nodiscard]] bool isPassableForShell() const;
    [[nodiscard]] bool hasEntity() const { return entity != nullptr;}

};