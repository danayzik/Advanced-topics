#include "AdaptiveTankAlgorithm.h"
#include "SnipingTank.h"
#include "ChasingTank.h"
namespace Algorithm_206038929_314620071 {
    void AdaptiveTankAlgorithm::roundTick() {
        roundCounter++;
        if (initialized) {
            battleInfo.roundTick();
        }
    }

    void AdaptiveTankAlgorithm::updateBattleInfo(BattleInfo &info) {
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


    ActionRequest AdaptiveTankAlgorithm::requestAction(ActionRequest action) {
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


    ActionRequest AdaptiveTankAlgorithm::getAction() {
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
