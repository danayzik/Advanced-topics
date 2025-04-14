#pragma once
#include <cmath>
#include <stdexcept>

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

inline Direction offSetToDirection(int dy, int dx){
    if (dy == 0 && dx == 0) throw std::invalid_argument("Zero offset has no direction.");
    double angle = atan2(dy, dx) * 180.0 / PI;
    if (angle < 0) angle += 360.0;
    int roundedAngle = static_cast<int>(round(angle / 45.0)) * 45 % 360;
    return static_cast<Direction>(roundedAngle);
}
inline int getDirectionDiff(Direction currDirection, Direction targetDirection){
    return (static_cast<int>(targetDirection) - static_cast<int>(currDirection) + 540) % 360 - 180;
}

inline int minimalRotationsNeeded(Direction currDirection, Direction targetDirection){
    if(currDirection == targetDirection) return 0;
    int diff = std::abs(getDirectionDiff(currDirection, targetDirection));
    return diff > 90 ? 2 : 1;
}


