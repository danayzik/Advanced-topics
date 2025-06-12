#pragma once
#include "FullBattleInfo.h"



class PlayerCommunicationModule{


private:
    static constexpr int getInfoCounterWhenAlone = 4;
    int tankCount = 0;
    int playerIndex;
    size_t rows;
    size_t cols;
    size_t maxSteps;
    size_t numShells;
    FullBattleInfo battleInfo;
    std::vector<int> tankIndices = {};
    bool seenAllMyTanks = false;
    std::vector<int> tankIndicesThatRecentlyDied{};
    void checkForDeadTanks();
public:
    virtual void updateTankInstructions() = 0;
    virtual void processSatelliteView(SatelliteView& satellite_view);
    virtual void processBattleInfoPreSending();
    virtual void processBattleInfoPostSending();
    virtual ~PlayerCommunicationModule() = default;



};