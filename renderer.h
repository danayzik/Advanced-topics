#pragma once
#include "cell.h"
#include "entities.h"
#include "vector"
class Renderer {

public:
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void drawGrid(const std::vector<std::vector<Cell>>& grid) = 0;
    virtual void drawCell(const Cell* cell) = 0;
    virtual void drawCells(const unordered_set<Cell*>& cells) = 0;
};
