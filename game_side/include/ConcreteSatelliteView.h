#pragma once

#include "SatelliteView.h"
#include <vector>
class ConcreteSatelliteView : public SatelliteView {
private:
    size_t rows;
    size_t cols;
    std::vector<std::vector<char>> visibleEntities;


public:
    ConcreteSatelliteView(size_t rows, size_t cols, std::vector<std::vector<char>> entities)
            : rows(rows), cols(cols), visibleEntities(std::move(entities)) {}
    [[nodiscard]] inline char getObjectAt(size_t x, size_t y) const override{
        return (x < cols && y < rows) ? visibleEntities[y][x] : '&';
    };

    ConcreteSatelliteView(const ConcreteSatelliteView&) = delete;
    ConcreteSatelliteView(ConcreteSatelliteView&&) = delete;
    ConcreteSatelliteView& operator=(const ConcreteSatelliteView&) = delete;
    ConcreteSatelliteView& operator=(ConcreteSatelliteView&&) = delete;
    ~ConcreteSatelliteView() override = default;


};