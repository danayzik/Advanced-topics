#include "SecondTankAlgorithm.h"
#include <queue>
#include <algorithm>
#include "ActionRequest.h"
#include "AlgorithmUtilities.h"
void SecondTankAlgorithm::roundTick() {
    roundCounter++;
    stepsSinceLastCalculation++;
    stepsSinceLastInfoRequest++;
    if(initialized){
        battleInfo.roundTick();
    }
}

void SecondTankAlgorithm::updateBattleInfo(BattleInfo &info) {
    auto& newInfo= dynamic_cast<FullBattleInfo&>(info);
    if(!initialized){
        battleInfo = newInfo;
        initialized = true;
    }
    else{
        newInfo.updateFromTankInfo(battleInfo);
        battleInfo = newInfo;
    }
}



ActionRequest SecondTankAlgorithm::requestAction(ActionRequest action) {
    auto& myTank = battleInfo.getMyTank();
    switch (action){ //Never requesting backward
        case ActionRequest::GetBattleInfo:
            stepsSinceLastInfoRequest = 0;
            break;
        case ActionRequest::DoNothing:
            break;
        case ActionRequest::MoveForward:
            battleInfo.moveMyTankForward();
            break;
        case ActionRequest::Shoot:
            myTank.fire();
            battleInfo.fireShellFromTank(myTank);
            break;
        default:
            myTank.rotate(rotateActionToAngle(action));
            break;
    }
    return action;
}


ActionRequest SecondTankAlgorithm::getAction() {
    roundTick();
    if(!initialized){
        stepsSinceLastInfoRequest = 0;
        return ActionRequest::GetBattleInfo;
    }

    if(stepsSinceLastInfoRequest > 3){
        return requestAction(ActionRequest::GetBattleInfo);
    }
    const auto& tank = battleInfo.getMyTank();
    if(tank.canFire())
        return requestAction(ActionRequest::Shoot);
    return requestAction(ActionRequest::MoveForward);
}


