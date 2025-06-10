#pragma once
#include "TankRoleAlgorithm.h"
#include "AlgorithmUtilities.h"

class ChasingTank : public TankRoleAlgorithm{
private:
    Coordinates closestTarget = {};
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
    std::vector<std::vector<HeapNode>> gridGraph;
    void resetGraph();
    void calculateGridPathsFromTarget(const FullBattleInfo& battleInfo);
    [[nodiscard]] std::optional<ActionRequest> getAdvancingToTargetAction(const FullBattleInfo& battleInfo) const;


public:
    explicit ChasingTank(const FullBattleInfo& battleInfo);
    ActionRequest getAction(const FullBattleInfo& battleInfo) override;
    void update(const FullBattleInfo& battleInfo) override;

};