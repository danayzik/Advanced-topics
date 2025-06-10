#pragma once
#include "ActionRequest.h"
#include "FullBattleInfo.h"



class TankRoleAlgorithm{
public:
    virtual ActionRequest getAction(const FullBattleInfo& battleInfo) = 0;
    virtual void update(const FullBattleInfo& battleInfo) = 0;
    virtual ~TankRoleAlgorithm() = default;
};