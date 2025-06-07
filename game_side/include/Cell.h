#pragma once
#include <unordered_set>
#include "EntityManager.h"
class GameEntity;

using std::unordered_set;

struct Cell {
    unordered_set<size_t> entitySet;
    [[nodiscard]] bool hasWall(const EntityManager& entityManager) const;
    Coordinates coords = {};
    Cell() = default;
    inline explicit Cell(Coordinates coords) : coords(coords){};
    inline void setCoordinates(Coordinates newCoords){coords = newCoords;}
    inline int y() const{return coords.y;}
    inline int x() const{return coords.x;}
    inline size_t xAsSizeT() const { return coords.xAsSizeT();}
    inline size_t yAsSizeT() const { return coords.yAsSizeT();}
};
