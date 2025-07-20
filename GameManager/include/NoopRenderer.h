#pragma once
#include "Renderer.h"
namespace GameManager_206038929_314620071 {
    class NoOpRenderer : public Renderer {
    public:
        void initialize() override {}

        void drawGrid([[maybe_unused]]const std::vector<std::vector<Cell>> &grid,
                      [[maybe_unused]]const EntityManager &entityManager) override {}
    };
}