#pragma once

#include "TankAlgorithm.h"

class FirstTankAlgorithm : public TankAlgorithm{
public:
    ActionRequest getAction() override;
    virtual void updateBattleInfo(BattleInfo& info) override;
};