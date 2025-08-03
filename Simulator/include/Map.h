#pragma once
#include <memory>
#include <string>
#include "ConcreteSatelliteView.h"


struct Map{
    size_t maxSteps;
    size_t numShells;
    size_t rows;
    size_t cols;
    std::string mapFileName = {};
    std::unique_ptr<SatelliteView> view = {};


    [[nodiscard]] std::unique_ptr<SatelliteView> getViewCopy() const{
        std::vector<std::vector<char>> entities(rows, std::vector<char>(cols, ' '));
        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                entities[y][x] = view->getObjectAt(x, y);
            }
        }
        return std::make_unique<UserCommon_206038929_314620071::ConcreteSatelliteView>(rows, cols, std::move(entities));
    }

    static std::string getStringFromView(SatelliteView& view, size_t rows, size_t cols){
        std::string finalView;
        finalView.reserve((cols + 1) * rows);
        for (size_t y = 0; y < rows; ++y) {
            for (size_t x = 0; x < cols; ++x) {
                finalView += view.getObjectAt(x, y);
            }
            finalView += '\n';
        }
        return finalView;
    }

};