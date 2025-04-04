#pragma once

#include "direction.h"
#include "game_entity.h"

class Tank : public GameEntity{
private:
    enum Direction currDirection = Up;
    int shellCount = 16;
    bool inReverseMode = false;
    bool isAlive = true;
    bool canFire = true;
    int stepsSinceLastShot = 100; // Placeholder initial value
    bool preparingReverseMode = false;

public:
    inline enum Direction getDirection() { return currDirection; }
    inline int getShellCount() { return shellCount; }
    inline bool isInReverse() { return inReverseMode; }
    inline bool isPreparingReverse() { return preparingReverseMode; }
    inline bool getCanFire() { return canFire; }
    void rotate(int rotationAmount);
    void fire();
    void putInReverse();
    void moveForward();
};
