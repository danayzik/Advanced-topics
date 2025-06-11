#include "PlayerOne.h"


void PlayerOne::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
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
    else{
        checkForDeadTanks();
        if(shouldCalculateShells(battleInfo.getTankIndex()))
            battleInfo.assumeEnemyShellsDirections(2);
    }
}

void PlayerOne::updateTankInstructions() {
    if(tankCount == 0)
        battleInfo.setRole(TankRole::Chasing);
    else
        battleInfo.setRole(TankRole::Sniping);
    battleInfo.setRequestInfoCounter(static_cast<int>(tankCount)+1);

}

void PlayerOne::checkForDeadTanks() {
    int aliveTanks = static_cast<int>(battleInfo.getFriendlyTanksCoordinates().size());
    bool tankDied = aliveTanks != tankCount;
    if(tankDied){
        for (int tankIndex : tankIndices) {
            if(!battleInfo.isFriendlyTankAlive(tankIndex))
                tankIndicesThatRecentlyDied.push_back(tankIndex);
        }
        tankCount = aliveTanks;
    }
}

bool PlayerOne::shouldCalculateShells(int tankIndex) {
    bool shouldCalculate = !appearsRightAfter(tankIndicesThatRecentlyDied, tankIndex);
    removeIfValIsAfter(tankIndicesThatRecentlyDied, tankIndex);
    return shouldCalculate;
}
