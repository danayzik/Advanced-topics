#include "PlayerOne.h"


void PlayerOne::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
    FullBattleInfo satelliteInfo{rows, cols, numShells, satellite_view, playerIndex};
    satelliteInfo.updateFromEarlierInfo(battleInfo);
    battleInfo = satelliteInfo;
    tank.updateBattleInfo(battleInfo);
}