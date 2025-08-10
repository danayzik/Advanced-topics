#include "Player_206038929_314620071.h"
#include "PlayerRegistration.h"



namespace Algorithm_206038929_314620071 {
    REGISTER_PLAYER(Player_206038929_314620071);
    void Player_206038929_314620071::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
        processSatelliteView(satellite_view);
        processBattleInfoPreSending();
        tank.updateBattleInfo(battleInfo);
        processBattleInfoPostSending();
    }


    void Player_206038929_314620071::processSatelliteView(SatelliteView &satellite_view) {
        FullBattleInfo satelliteInfo{rows, cols, numShells, satellite_view, playerIndex};
        satelliteInfo.updateFromEarlierInfo(battleInfo);
        battleInfo = std::move(satelliteInfo);
    }

    void Player_206038929_314620071::updateTankInstructions() {
        if (tankCount <= 0)
            battleInfo.setRole(TankRole::Sniping);
        else
            battleInfo.setRole(TankRole::Chasing);
        battleInfo.setRequestInfoCounter(static_cast<int>(tankCount) + 1);
    }

    void Player_206038929_314620071::processBattleInfoPostSending() {
        int index = battleInfo.getTankIndex();
        if (!seenAllMyTanks) {
            if (!contains(tankIndices, index)) {
                tankCount++;
                tankIndices.push_back(battleInfo.getTankIndex());
            } else {
                seenAllMyTanks = true;
            }
        } else {

            battleInfo.assumeEnemyShellsDirections(2);
        }
    }



    void Player_206038929_314620071::processBattleInfoPreSending() {
        if (!seenAllMyTanks)
            updateTankInstructions();
        else {
            tankCount = static_cast<int>(battleInfo.getFriendlyTanks().size());
            int getInfoCounter = tankCount == 1 ? getInfoCounterWhenAlone : tankCount;
            battleInfo.setRequestInfoCounter(getInfoCounter);
        }
    }


}