#pragma once
#include "renderer.h"

class NoOpRenderer : public Renderer {
public:
    void initialize() override {}
    void drawCell(const Cell* cell) override {}
    void drawCells(const unordered_set<Cell*>& cells) override {}
};
