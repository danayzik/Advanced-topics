#pragma once
#include <cassert>
#include <iostream>
#include "Entities.h"
#include <algorithm>


namespace GameManager_206038929_314620071::EntityUtils {

    template<typename T>
    inline T *entityCast(GameEntity *e) {
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
    inline const T *entityCast(const GameEntity *e) {
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
