#pragma once
#include "TankRoleAlgorithm.h"
#include "AlgorithmUtilities.h"

class SnipingTank : public TankRoleAlgorithm{
private:

    std::optional<Coordinates> targetCoords{};
    std::optional<ActionRequest> rotationTowardsTarget(const FullBattleInfo &battleInfo);
    inline void acquireTarget(const FullBattleInfo &battleInfo);
public:
    explicit SnipingTank();
    ActionRequest getAction(const FullBattleInfo& battleInfo) override;
    void update(const FullBattleInfo& battleInfo) override;

};