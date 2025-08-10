#pragma once
#include <functional>
#include <cmath>
#include <cstdlib>
namespace UserCommon_206038929_314620071 {
    struct Coordinates {
        int y;
        int x;

        Coordinates(int y, int x) : y(y), x(x) {}

        Coordinates(size_t y, size_t x) : y(static_cast<int>(y)), x(static_cast<int>(x)) {}

        Coordinates() = default;

        [[nodiscard]] size_t yAsSizeT() const { return static_cast<size_t>(y); }

        [[nodiscard]] size_t xAsSizeT() const { return static_cast<size_t>(x); }

        bool operator==(const Coordinates &other) const {
            return y == other.y && x == other.x;
        }

        bool operator!=(const Coordinates &other) const {
            return y != other.y || x != other.x;
        }

        [[nodiscard]] inline int distanceToOtherCoord(Coordinates coord, size_t rows, size_t cols) const {
            int intRows = static_cast<int>(rows);
            int intCols = static_cast<int>(cols);
            int dy = std::min(std::abs(coord.y - y), intRows - std::abs(coord.y - y));
            int dx = std::min(std::abs(coord.x - x), intCols - std::abs(coord.x - x));
            return std::max(dy, dx);
        }

    };

    struct CoordinatesHash {
        size_t operator()(const Coordinates &coord) const {
            return std::hash<int>()(coord.y) ^ (std::hash<int>()(coord.x) << 1);
        }
    };
}