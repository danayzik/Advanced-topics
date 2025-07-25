#include "Entities.h"

namespace GameManager_206038929_314620071 {
    void Tank::rotate(int rotationAmount) {
        currDirection = DirectionUtils::rotateDirection(currDirection, rotationAmount);

    }

    void Tank::setMode(TankMode newMode) {
        mode = newMode;
        if (newMode == PreparingReverse) {
            preparingReverseCounter = 2;
        } else {
            preparingReverseCounter = -1;
        }
    }

    Tank::Tank(size_t entityId, size_t y, size_t x, Direction dir, int playerIndex, int tankIndex, size_t shellCount)
            : GameEntity(entityId, y, x), currDirection(dir), playerIndex(playerIndex), tankIndex(tankIndex),
              shellCount(shellCount) {}


    ActionRequest Tank::consumeAction() {
        ActionRequest action = nextStepAction;
        nextStepAction = ActionRequest::DoNothing;
        return action;
    }

    void Tank::tickUpdate() {
        if (mode == JustEnteredReverse) {
            mode = ReverseMode;
        }
        stepsSinceLastShot++;

        preparingReverseCounter--;
        if (preparingReverseCounter == 0) {
            mode = JustEnteredReverse;
        }
    }

    bool Tank::canFire() const {
        return shellCount > 0 && stepsSinceLastShot > firingCooldown;
    }

    void Tank::fire() {
        shellCount--;
        stepsSinceLastShot = 0;
    }

    void Tank::setAction(ActionRequest action) {
        nextStepAction = action;
    }

}