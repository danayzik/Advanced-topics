#pragma once
#include <functional>

struct Coordinates {
    int y;
    int x;

    Coordinates(int y, int x) : y(y), x(x) {}
    Coordinates(size_t y, size_t x) : y(static_cast<int>(y)), x(static_cast<int>(x)) {}
    Coordinates() = default;
    [[nodiscard]] size_t yAsSizeT() const { return static_cast<size_t>(y); }
    [[nodiscard]] size_t xAsSizeT() const { return static_cast<size_t>(x); }

    bool operator==(const Coordinates& other) const {
        return y == other.y && x == other.x;
    }
};

struct CoordinatesHash {
    size_t operator()(const Coordinates& coord) const {
        return std::hash<int>()(coord.y) ^ (std::hash<int>()(coord.x) << 1);
    }
};