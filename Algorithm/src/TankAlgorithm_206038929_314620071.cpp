#include "TankAlgorithm_206038929_314620071.h"
#include "SnipingTank.h"
#include "ChasingTank.h"
#include "TankAlgorithmRegistration.h"



namespace Algorithm_206038929_314620071 {
    REGISTER_TANK_ALGORITHM(TankAlgorithm_206038929_314620071);
    void TankAlgorithm_206038929_314620071::roundTick() {
        roundCounter++;
        if (initialized) {
            battleInfo.roundTick();
        }
    }

    void TankAlgorithm_206038929_314620071::updateBattleInfo(BattleInfo &info) {
        auto &newInfo = dynamic_cast<FullBattleInfo &>(info);
        if (!initialized) {
            battleInfo = newInfo;
            newInfo.setTankIndex(tankIndex);
            switch (battleInfo.getRole()) {
                case TankRole::Sniping:
                    algo = std::make_unique<SnipingTank>();
                    break;
                case TankRole::Chasing:
                    algo = std::make_unique<ChasingTank>(battleInfo);
                    break;
            }
            algo->update(battleInfo);
            initialized = true;
        } else {
            battleInfo.setTankIndex(tankIndex);
            newInfo.updateFromTankInfo(battleInfo);
            battleInfo = newInfo;
            algo->update(battleInfo);
        }

    }


    ActionRequest TankAlgorithm_206038929_314620071::requestAction(ActionRequest action) {
        auto &myTank = battleInfo.getMyTank();
        switch (action) { //I know all algos currently never request backwards, add handling and tracking to it if needed
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


    ActionRequest TankAlgorithm_206038929_314620071::getAction() {
        roundTick();
        if (!initialized) {
            return ActionRequest::GetBattleInfo;
        }
        if (battleInfo.shouldRequestInfo())
            return ActionRequest::GetBattleInfo;

        ActionRequest action = algo->getAction(battleInfo);
        return requestAction(action);
    }


}
