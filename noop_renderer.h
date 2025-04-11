#pragma once
#include "renderer.h"

class NoOpRenderer : public Renderer {
public:
    void initialize(int rows, int cols) override {}
    void render() override {}
    void drawTank(int y, int x, int playerNumber) override {}
    void drawWall(int y, int x) override {}
    void drawMine(int y, int x) override {}
};
