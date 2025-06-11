#include "ObservedEntities.h"


void ObservedTank::fire() {
    shellCount--;
    stepsSinceLastShot = 0;
}

bool ObservedTank::canFire() const {
    return shellCount > 0 && stepsSinceLastShot > firingCooldown;
}

void ObservedTank::rotate(int rotationAmount) {
    if(!assumedDirection)
        return;
    int newAngle = (DirectionUtils::directionToInt(assumedDirection.value()) + rotationAmount) % 360;
    if (newAngle < 0) newAngle += 360;
    assumedDirection = DirectionUtils::intToDirection(newAngle).value();
}

void ObservedTank::tickUpdate() {
    if(mode == JustEnteredReverse){
        mode = ReverseMode;
    }
    stepsSinceLastShot++;

    preparingReverseCounter--;
    if(preparingReverseCounter == 0){
        mode = JustEnteredReverse;
    }
}