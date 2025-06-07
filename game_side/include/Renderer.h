#pragma once
#include "Cell.h"
#include "Entities.h"
#include "vector"
class Renderer {

public:
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void drawGrid(const std::vector<std::vector<Cell>>& grid, const EntityManager& entityManager) = 0;
};
