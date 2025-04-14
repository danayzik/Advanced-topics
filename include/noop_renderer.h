#pragma once
#include "renderer.h"

class NoOpRenderer : public Renderer {
public:
    void initialize() override {}
    void drawGrid(const std::vector<std::vector<Cell>>& grid) override {}
};
