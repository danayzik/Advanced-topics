#include "PlayerOne.h"


void PlayerOne::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    FullBattleInfo satelliteInfo{rows, cols, numShells, satellite_view, playerIndex};
    satelliteInfo.updateFromEarlierInfo(battleInfo);
    battleInfo = satelliteInfo;
    size_t currentTankCount = tankIndices.size();

    tank.updateBattleInfo(battleInfo);
    int index = battleInfo.getTankIndex();
    if (!contains(tankIndices, index)) {
        tankIndices.push_back(battleInfo.getTankIndex());
    }

}

TankRole PlayerOne::tankCountToRole() const {
    size_t tankCount = tankIndices.size();
    if(tankCount == 0)
        return TankRole::Chasing;
    return TankRole::Sniping;

}