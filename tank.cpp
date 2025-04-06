#include "entities.h"
#include "direction.h"


void Tank::rotate(int rotationAmount) {
    int newAngle = (static_cast<int>(currDirection) + rotationAmount) % 360;
    if (newAngle < 0) newAngle += 360;
    currDirection = static_cast<Direction>(newAngle);

}

Tank::Tank(int y, int x, Cell *cell) : GameEntity(y, x, cell){}

Action Tank::consumeAction() {
    Action action = nextStepAction;
    nextStepAction = NoAction;
    return action;
}

void Tank::setAction(Action action) {
    nextStepAction = action;
}

bool Tank::collide() {
    //Destroy tank
    return true;
}