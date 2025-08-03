#pragma once

#include "SatelliteView.h"
#include <vector>
#include "Coordinates.h"
#include <optional>

namespace UserCommon_206038929_314620071 {

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

        inline void setRequestingTank(const Coordinates& requestingTankCoords, char requestingTankSymbol) {
            if (lastRequestingTankCoords.has_value()) {
                Coordinates oldCoords = lastRequestingTankCoords.value();
                visibleEntities[oldCoords.yAsSizeT()][oldCoords.xAsSizeT()] = lastRequestingTankSymbol;
            }
            lastRequestingTankCoords = requestingTankCoords;
            lastRequestingTankSymbol = requestingTankSymbol;
            visibleEntities[requestingTankCoords.yAsSizeT()][requestingTankCoords.xAsSizeT()] = '%';
        }

        ConcreteSatelliteView(const ConcreteSatelliteView &) = delete;

        ConcreteSatelliteView(ConcreteSatelliteView &&) = delete;

        ConcreteSatelliteView &operator=(const ConcreteSatelliteView &) = delete;

        ConcreteSatelliteView &operator=(ConcreteSatelliteView &&) = delete;

        ~ConcreteSatelliteView() override = default;


    };
}