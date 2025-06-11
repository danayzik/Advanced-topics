#include "PlayerTwo.h"


void PlayerTwo::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    FullBattleInfo satelliteInfo{rows, cols, numShells, satellite_view, playerIndex};

    satelliteInfo.updateFromEarlierInfo(battleInfo);
    battleInfo = satelliteInfo;
    if(!seenAllMyTanks)
        updateTankInstructions();
    else{
        battleInfo.setRequestInfoCounter(tankCount);
    }
    tank.updateBattleInfo(battleInfo);
    int index = battleInfo.getTankIndex();
    if(!seenAllMyTanks) {
        if (!contains(tankIndices, index)) {
            tankCount++;
            tankIndices.push_back(battleInfo.getTankIndex());
        }
        else {
            seenAllMyTanks = true;
        }
    }

}

void PlayerTwo::updateTankInstructions() {
    if(tankCount <= 1)
        battleInfo.setRole(TankRole::Sniping);
    else
        battleInfo.setRole(TankRole::Chasing);
    battleInfo.setRequestInfoCounter(4);
}