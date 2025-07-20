#pragma once
#include "TankRoleAlgorithm.h"
#include "AlgorithmUtilities.h"
namespace Algorithm_206038929_314620071 {
    class ChasingTank : public TankRoleAlgorithm {
    private:
        Coordinates closestTarget = {};
        std::vector<std::vector<HeapNode>> gridGraph;

        void resetGraph();

        void calculateGridPathsFromTarget(const FullBattleInfo &battleInfo);

        [[nodiscard]] std::optional<ActionRequest> getAdvancingToTargetAction(const FullBattleInfo &battleInfo) const;


    public:
        explicit ChasingTank(const FullBattleInfo &battleInfo);

        ActionRequest getAction(const FullBattleInfo &battleInfo) override;

        void update(const FullBattleInfo &battleInfo) override;

    };
}