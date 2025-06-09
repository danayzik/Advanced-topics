#pragma once
#include <cassert>
#include "Entities.h"
#include "ObservedEntities.h"



namespace EntityUtils {


    template<typename T>
    T *entityCast(ObservedEntity *e) {
        assert(e != nullptr);
        if constexpr (std::is_same_v<T, ObservedTank>)
            assert(e->isTank());
        else if constexpr (std::is_same_v<T, ObservedShell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, ObservedWall>)
            assert(e->isWall());
        return static_cast<T *>(e);
    }

    template<typename T>
    const T *entityCast(const ObservedEntity *e) {
        assert(e != nullptr);
        if constexpr (std::is_same_v<T, ObservedTank>)
            assert(e->isTank());
        else if constexpr (std::is_same_v<T, ObservedShell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, ObservedWall>)
            assert(e->isWall());
        return static_cast<const T*>(e);
    }

    template<typename T>
    T *entityCast(GameEntity *e) {
        assert(e != nullptr);
        if constexpr (std::is_same_v<T, Tank>)
            assert(e->isTank());
        else if constexpr (std::is_same_v<T, Shell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, Wall>)
            assert(e->isWall());
        else if constexpr (std::is_same_v<T, Mine>)
            assert(e->isMine());
        return static_cast<T *>(e);
    }

    template<typename T>
    const T *entityCast(const GameEntity *e) {
        assert(e != nullptr);
        if constexpr (std::is_same_v<T, Tank>)
            assert(e->isTank());
        else if constexpr (std::is_same_v<T, Shell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, Wall>)
            assert(e->isWall());
        else if constexpr (std::is_same_v<T, Mine>)
            assert(e->isMine());
        return static_cast<const T*>(e);
    }


}