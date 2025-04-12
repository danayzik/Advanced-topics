#pragma once
#include "renderer.h"

class NoOpRenderer : public Renderer {
public:
    void initialize() override {}
    void drawCell(const Cell* cell, int y, int x) override {}
};
