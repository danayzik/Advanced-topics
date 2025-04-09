#pragma once
#include <cmath>
constexpr double PI = 3.14159265358979323846;



enum Direction {
    Right     = 0,
    DownRight = 45,
    Down      = 90,
    DownLeft  = 135,
    Left      = 180,
    UpLeft    = 225,
    Up        = 270,
    UpRight   = 315
};

inline Direction getOppositeDirection(Direction dir) {
    int opposite = (static_cast<int>(dir) + 180) % 360;
    return static_cast<Direction>(opposite);
}
inline std::pair<int, int> directionToCoordinatesOffset(Direction dir){
    double radians = dir * PI / 180.0;
    return {static_cast<int>(round(sin(radians))), static_cast<int>(round(cos(radians)))};
}

