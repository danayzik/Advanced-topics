#pragma once

#include "SatelliteView.h"
#include <vector>
#include "Entities.h"
namespace GameManager_206038929_314620071 {

    class ConcreteSatelliteView : public SatelliteView {
    private:
        size_t rows;
        size_t cols;
        std::vector<std::vector<char>> visibleEntities;
        std::optional<Coordinates> lastRequestingTankCoords = {};
        char lastRequestingTankSymbol = {};

    public:
        ConcreteSatelliteView(size_t rows, size_t cols, std::vector<std::vector<char>>&& entities)
                : rows(rows), cols(cols), visibleEntities(std::move(entities)) {}

        [[nodiscard]] inline char getObjectAt(size_t x, size_t y) const override {
            return (x < cols && y < rows) ? visibleEntities[y][x] : '&';
        };

        inline void setRequestingTank(Tank &tank) {
            if (lastRequestingTankCoords.has_value()) {
                Coordinates oldCoords = lastRequestingTankCoords.value();
                visibleEntities[oldCoords.yAsSizeT()][oldCoords.xAsSizeT()] = lastRequestingTankSymbol;
            }
            Coordinates coords = tank.getCoords();
            lastRequestingTankCoords = coords;
            lastRequestingTankSymbol = tank.getSymbol();
            visibleEntities[coords.yAsSizeT()][coords.xAsSizeT()] = '%';
        }

        ConcreteSatelliteView(const ConcreteSatelliteView &) = delete;

        ConcreteSatelliteView(ConcreteSatelliteView &&) = delete;

        ConcreteSatelliteView &operator=(const ConcreteSatelliteView &) = delete;

        ConcreteSatelliteView &operator=(ConcreteSatelliteView &&) = delete;

        ~ConcreteSatelliteView() override = default;


    };
}