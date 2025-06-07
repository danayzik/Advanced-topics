#pragma once
#include <cmath>
#include <stdexcept>
#include <optional>
#include "Coordinates.h"

enum class Direction {
    Right     = 0,
    DownRight = 45,
    Down      = 90,
    DownLeft  = 135,
    Left      = 180,
    UpLeft    = 225,
    Up        = 270,
    UpRight   = 315
};

namespace DirectionUtils {

    constexpr double PI = 3.14159265358979323846;

    inline int directionToInt(Direction dir){return static_cast<int>(dir);}

    inline std::optional<Direction> intToDirection(int angle) {
        switch(angle) {
            case 0:   return Direction::Right;
            case 45:  return Direction::DownRight;
            case 90:  return Direction::Down;
            case 135: return Direction::DownLeft;
            case 180: return Direction::Left;
            case 225: return Direction::UpLeft;
            case 270: return Direction::Up;
            case 315: return Direction::UpRight;
            default:  return std::nullopt;
        }
    }

    inline Direction getOppositeDirection(Direction dir) {
        int opposite = (directionToInt(dir) + 180) % 360;
        return static_cast<Direction>(opposite);
    }

    inline std::pair<int, int> directionToCoordinatesOffset(Direction dir) {
        double radians = directionToInt(dir) * PI / 180.0;
        return {static_cast<int>(round(sin(radians))), static_cast<int>(round(cos(radians)))};
    }
    inline Coordinates nextCoordinate(Direction dir, Coordinates current, size_t rows, size_t cols){
        auto[dy, dx] = directionToCoordinatesOffset(dir);
        int newY = (current.y + dy + static_cast<int>(rows)) % static_cast<int>(rows);
        int newX = (current.x + dx + static_cast<int>(cols)) % static_cast<int>(cols);
        return Coordinates{newY, newX};
    }

    inline Direction getDirectionBetweenPoints(int y1, int x1, int y2, int x2) {
        double dy = y2 - y1;
        double dx = x2 - x1;
        double angle = std::atan2(dy, dx) * (180.0 / PI);
        if (angle < 0) angle += 360.0;
        int snapped = static_cast<int>(std::round(angle / 45.0)) * 45 % 360;

        return static_cast<Direction>(snapped);
    }


    inline Direction offSetToDirection(int dy, int dx) {
        if (dy == 0 && dx == 0) throw std::invalid_argument("Zero offset has no direction.");
        double angle = atan2(dy, dx) * 180.0 / PI;
        if (angle < 0) angle += 360.0;
        int roundedAngle = static_cast<int>(round(angle / 45.0)) * 45 % 360;
        return static_cast<Direction>(roundedAngle);
    }

    inline int getDirectionDiff(Direction currDirection, Direction targetDirection) {
        return (directionToInt(targetDirection) - directionToInt(currDirection) + 540) % 360 - 180;
    }

    inline int minimalRotationsNeeded(Direction currDirection, Direction targetDirection) {
        if (currDirection == targetDirection) return 0;
        int diff = std::abs(getDirectionDiff(currDirection, targetDirection));
        return diff > 90 ? 2 : 1;
    }

}
