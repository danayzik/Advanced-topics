#pragma once
#include "Cell.h"
#include "Entities.h"
#include "vector"
namespace GameManager_206038929_314620071 {
    class Renderer {

    public:
        virtual ~Renderer() = default;

        virtual void initialize() = 0;

        virtual void drawGrid(const std::vector<std::vector<Cell>> &grid, const EntityManager &entityManager) = 0;
    };
}