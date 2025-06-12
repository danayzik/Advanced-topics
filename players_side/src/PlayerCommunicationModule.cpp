#include "PlayerCommunicationModule.h"


void PlayerCommunicationModule::processSatelliteView(SatelliteView &satellite_view) {
    FullBattleInfo satelliteInfo{rows, cols, numShells, satellite_view, playerIndex};
    satelliteInfo.updateFromEarlierInfo(battleInfo);
    battleInfo = satelliteInfo;

}


void PlayerCommunicationModule::processBattleInfoPreSending() {
    if(!seenAllMyTanks)
        updateTankInstructions();
    else{
        int getInfoCounter = tankCount == 1 ? getInfoCounterWhenAlone : tankCount;
        battleInfo.setRequestInfoCounter(getInfoCounter);
    }

}

void PlayerCommunicationModule::processBattleInfoPostSending() {
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
        removeIfValIsAfter(tankIndicesThatRecentlyDied, index);
    }
}

void PlayerCommunicationModule::checkForDeadTanks() {
    int aliveTanks = static_cast<int>(battleInfo.getFriendlyTanksCoordinates().size());
    bool tanksDied = aliveTanks != tankCount;
    if(tanksDied){
        for (int tankIndex : tankIndices) {
            if(!battleInfo.isFriendlyTankAlive(tankIndex))
                tankIndicesThatRecentlyDied.push_back(tankIndex);
        }
        tankCount = aliveTanks;
    }
}

