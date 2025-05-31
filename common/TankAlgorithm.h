#pragma once
#include "ActionRequest.h"
#include "BattleInfo.h"

class TankAlgorithm {
public:
    virtual ~TankAlgorithm() = default;
    virtual ActionRequest getAction() = 0;
    virtual void updateBattleInfo(BattleInfo& info) = 0;
};
