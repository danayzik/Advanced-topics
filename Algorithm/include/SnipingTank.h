#pragma once
#include "TankRoleAlgorithm.h"
#include "AlgorithmUtilities.h"
namespace Algorithm_206038929_314620071{
    class SnipingTank : public TankRoleAlgorithm {
    private:

        std::optional<Coordinates> targetCoords{};

        std::optional<ActionRequest> rotationTowardsTarget(const FullBattleInfo &battleInfo);

        inline void acquireTarget(const FullBattleInfo &battleInfo);

    public:
        explicit SnipingTank();

        ActionRequest getAction(const FullBattleInfo &battleInfo) override;

        [[nodiscard]] int getDistanceFromTarget(const FullBattleInfo &battleInfo) const override;

        void update(const FullBattleInfo &battleInfo) override;

        [[nodiscard]] TankRole getRole() const override{ return TankRole::Sniping;}

    };
}