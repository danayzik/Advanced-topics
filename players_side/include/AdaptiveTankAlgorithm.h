#pragma once

#include "TankAlgorithm.h"
#include "FullBattleInfo.h"
#include "AlgorithmUtilities.h"
#include "TankRoleAlgorithm.h"
class AdaptiveTankAlgorithm : public TankAlgorithm{
private:
    FullBattleInfo battleInfo = {};
    int playerIndex;
    int tankIndex;
    size_t roundCounter = 0;
    std::unique_ptr<TankRoleAlgorithm> algo = nullptr;
    void roundTick();
    bool initialized = false;

    ActionRequest requestAction(ActionRequest action);

public:

    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    AdaptiveTankAlgorithm(int playerIndex, int tankIndex) : playerIndex(playerIndex), tankIndex(tankIndex){}




};