#pragma once

#include "TankAlgorithm.h"
#include "FullBattleInfo.h"
#include "AlgorithmUtilities.h"

class FirstTankAlgorithm : public TankAlgorithm{
private:
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
    size_t roundCounter = 0;
    void roundTick();

    std::vector<std::vector<HeapNode>> gridGraph;
    int stepsSinceLastInfoRequest = 1000; //placeholder initial values
    int stepsSinceLastCalculation = 1000;
    bool initialized = false;
    void initGraph();
    [[nodiscard]] ActionRequest getBestMovement() ;
    void resetGraph();
    ActionRequest requestAction(ActionRequest action);


    void calculateGridPathsFromTarget(Coordinates targetCoords);
public:


    ActionRequest getAction() override;
    void updateBattleInfo(BattleInfo& info) override;
    FirstTankAlgorithm(int playerIndex, int tankIndex) : playerIndex(playerIndex), tankIndex(tankIndex){}




};