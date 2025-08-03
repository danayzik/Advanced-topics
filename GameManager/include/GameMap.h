#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include "Cell.h"
#include "Direction.h"
#include "Entities.h"
#include "EntityManager.h"
#include "SatelliteView.h"
#include "ConcreteSatelliteView.h"

namespace GameManager_206038929_314620071 {
    using std::unordered_set;
    using std::vector;
    using std::string;

    class GameManager;



    class GameMap {
    private:
        vector<vector<Cell>> grid = {};
        unordered_set<size_t> shellsIds = {};
        unordered_set<size_t> tankIds = {};
        EntityManager entityManager = {};
        size_t rows = 0;
        size_t cols = 0;

        Coordinates getNewPosition(const GameEntity &entity, Direction dir) const;

        void resolveCollisions(const unordered_set<Cell *> &dirtyCells);

        [[nodiscard]] inline Cell &getCell(Coordinates coords) { return grid[coords.yAsSizeT()][coords.xAsSizeT()]; }

        [[nodiscard]] inline Cell &getCell(size_t y, size_t x) { return grid[y][x]; }

        [[nodiscard]] inline const Cell &
        getCell(Coordinates coords) const { return grid[coords.yAsSizeT()][coords.xAsSizeT()]; }

        [[nodiscard]] inline const Cell &getCell(size_t y, size_t x) const { return grid[y][x]; }

        static void handleBadCharacter(size_t y, size_t x);
        void handleCell(size_t y, size_t x, char cell, int& tankCount, size_t numShells);


    public:
        GameMap() = default;

        bool tankCanMoveInDirection(const Tank &tank, Direction dir) const;

        void moveShells();

        void fireShell(const Tank &tank);

        void checkCollisions();

        void tanksAboutToCollide();

        void shellsAboutToCollide();

        void moveEntity(GameEntity &entity, Direction dir);

        inline Tank &getTank(size_t tankEntityIndex) {
            return dynamic_cast<Tank &>(entityManager.getEntity(tankEntityIndex));
        }

        std::unique_ptr<ConcreteSatelliteView> getSatelliteView();

        [[nodiscard]] inline size_t getRows() const { return rows; }

        [[nodiscard]] inline size_t getCols() const { return cols; }

        [[nodiscard]] inline const unordered_set<size_t>& getTankIds() const { return tankIds;}


        inline bool isTankAlive(size_t entityIndex) const { return entityManager.isTankAlive(entityIndex); }

        GameMap(const GameMap &) = delete;

        GameMap &operator=(const GameMap &) = delete;

        GameMap(GameMap &&) = delete;

        GameMap &operator=(GameMap &&) = delete;

        ~GameMap() = default;

        void buildMap(const SatelliteView& view, size_t height, size_t width, size_t numShells);


    };
}