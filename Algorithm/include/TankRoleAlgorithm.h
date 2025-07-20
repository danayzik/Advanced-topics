#pragma once
#include "ActionRequest.h"
#include "FullBattleInfo.h"

namespace Algorithm_206038929_314620071 {

    class TankRoleAlgorithm {
    public:
        virtual ActionRequest getAction(const FullBattleInfo &battleInfo) = 0;

        virtual void update(const FullBattleInfo &battleInfo) = 0;

        virtual ~TankRoleAlgorithm() = default;
    };
}