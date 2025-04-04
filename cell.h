#pragma once

#include <list>
using std::list;
struct Cell {
    list<GameEntity*> entityList;
    inline bool isEmpty() const{
        return entityList.empty();
    }
};
