#include "tank.h"
#include "direction.h"


void Tank::rotate(int rotationAmount) {
    int newAngle = (static_cast<int>(dir) + rotationAmountr) % 360;
    if (newAngle < 0) newAngle += 360;

    return static_cast<Direction>(newAngle);
}