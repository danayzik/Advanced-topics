#pragma once
#include "cell.h"
#include "entities.h"
class Renderer {

public:
    virtual ~Renderer() = default;

    virtual void initialize() = 0;
    virtual void render() = 0;
    virtual void drawTank(int y, int x, int playerNumber) = 0;
    virtual void drawWall(int y, int x) = 0;
    virtual void drawMine(int y, int x) = 0;
    virtual void drawCell(const Cell* cell, int y, int x) = 0;
};
