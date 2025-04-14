#include "../include/entities.h"
#include "../include/direction.h"


void Tank::rotate(int rotationAmount) {
    int newAngle = (static_cast<int>(currDirection) + rotationAmount) % 360;
    if (newAngle < 0) newAngle += 360;
    currDirection = static_cast<Direction>(newAngle);
}

void Tank::setMode(TankMode newMode){
    mode = newMode;
    if(newMode == PreparingReverse){
        preparingReverseCounter = 2;
    }
}

Tank::Tank(int y, int x, Cell *cell) : GameEntity(y, x, cell){}

Action Tank::consumeAction() {
    Action action = nextStepAction;
    nextStepAction = NoAction;
    return action;
}

void Tank::tickUpdate() {
    if(mode == JustEnteredReverse){
        mode = ReverseMode;
    }
    stepsSinceLastShot++;

    preparingReverseCounter--;
    if(preparingReverseCounter == 0){
        mode = JustEnteredReverse;
    }
}

bool Tank::canFire() const {
    return shellCount>0 && stepsSinceLastShot>4;
}

void Tank::fire() {
    shellCount--;
    stepsSinceLastShot = 0;
}

void Tank::setAction(Action action) {
    nextStepAction = action;
}

