#pragma once

#include "TankAlgorithm.h"
#include <vector>
#include "FullBattleInfo.h"

class SecondTankAlgorithm : public TankAlgorithm{
public:
    const std::vector<std::pair<int, int>> directions = {
            {0, 1},
            {1, 1},
            {1, 0},
            {1, -1},
            {0, -1},
            {-1, -1},
            {-1, 0},
            {-1, 1}
    };
    FullBattleInfo battleInfo = {};
    int playerIndex;
    int tankIndex;
    bool initialized = false;
    int stepsSinceLastInfoRequest = 1000; //placeholder initial values
    int stepsSinceLastCalculation = 1000;
    size_t roundCounter = 0;
    void roundTick();
    ActionRequest requestAction(ActionRequest action);
    ActionRequest getAction() override;
    virtual void updateBattleInfo(BattleInfo& info) override;
    SecondTankAlgorithm(int playerIndex, int tankIndex) : playerIndex(playerIndex), tankIndex(tankIndex){}

};