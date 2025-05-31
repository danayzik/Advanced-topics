#pragma once

#include "TankAlgorithm.h"

class SecondTankAlgorithm : public TankAlgorithm{
public:

    ActionRequest getAction() override;
    virtual void updateBattleInfo(BattleInfo& info) override;
};