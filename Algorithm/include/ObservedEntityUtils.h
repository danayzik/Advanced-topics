#pragma once
#include <cassert>
#include <iostream>
#include "ObservedEntities.h"
#include <algorithm>

namespace Algorithm_206038929_314620071::EntityUtils {



    template<typename T>
    inline T *entityCast(ObservedEntity *e) {

        assert(e != nullptr);
        if constexpr (std::is_same_v<T, ObservedTank>)assert(e->isTank());
        else if constexpr (std::is_same_v<T, ObservedShell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, ObservedWall>)
            assert(e->isWall());
        return static_cast<T *>(e);
    }

    template<typename T>
    inline const T *entityCast(const ObservedEntity *e) {
        assert(e != nullptr);
        if constexpr (std::is_same_v<T, ObservedTank>)
            assert(e->isTank());
        else if constexpr (std::is_same_v<T, ObservedShell>)
            assert(e->isShell());
        else if constexpr (std::is_same_v<T, ObservedWall>)
            assert(e->isWall());
        return static_cast<const T *>(e);
    }

    template<typename T>
    inline bool contains(const std::vector<T> &vec, const T &value) {
        return std::find(vec.begin(), vec.end(), value) != vec.end();
    }

    inline bool appearsRightAfter(const std::vector<int> &vec, int val) {
        for (size_t i = 0; i < vec.size(); ++i)
            if (vec[(i + 1) % vec.size()] == val)
                return true;
        return false;
    }

    inline void removeIfValIsAfter(std::vector<int> &vec, int val) {
        if (vec.size() <= 1) return;

        std::vector<int> result;
        for (size_t i = 0; i < vec.size(); ++i) {
            size_t next = (i + 1) % vec.size();
            if (vec[next] != val)
                result.push_back(vec[i]);
        }
        vec = std::move(result);
    }


}