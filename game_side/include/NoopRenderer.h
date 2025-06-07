#pragma once
#include "Renderer.h"

class NoOpRenderer : public Renderer {
public:
    void initialize() override {}
    void drawGrid(const std::vector<std::vector<Cell>>& grid, const EntityManager& entityManager) override {}
};
