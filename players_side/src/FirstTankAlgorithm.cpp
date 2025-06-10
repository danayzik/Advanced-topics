#include "FirstTankAlgorithm.h"


void FirstTankAlgorithm::roundTick() {
    roundCounter++;
    stepsSinceLastInfoRequest++;
    if(initialized){
        battleInfo.roundTick();
    }
}

void FirstTankAlgorithm::updateBattleInfo(BattleInfo &info) {
    stepsSinceLastInfoRequest = 0;
    auto& newInfo= dynamic_cast<FullBattleInfo&>(info);
    if(!initialized){
        battleInfo = newInfo;
        battleInfo.setTankIndex(tankIndex);
        //Craft algo
    }
    else{
        battleInfo.setTankIndex(tankIndex);
        newInfo.updateFromTankInfo(battleInfo);
        battleInfo = newInfo;
        algo->update(battleInfo);
    }
}



ActionRequest FirstTankAlgorithm::requestAction(ActionRequest action) {
    auto& myTank = battleInfo.getMyTank();
    switch (action){ //I know all algos currently never request backwards
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


ActionRequest FirstTankAlgorithm::getAction() {
    roundTick();
    if(!initialized){
        stepsSinceLastInfoRequest = 0;
        return ActionRequest::GetBattleInfo;
    }

    if(stepsSinceLastInfoRequest > 3){
        return requestAction(ActionRequest::GetBattleInfo);
    }
    ActionRequest action = algo->getAction(battleInfo);
    return requestAction(action);
}



