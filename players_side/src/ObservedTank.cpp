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
    assumedDirection = DirectionUtils::rotateDirection(assumedDirection.value(), rotationAmount);
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