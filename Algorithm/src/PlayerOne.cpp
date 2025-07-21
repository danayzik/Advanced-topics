#include "PlayerOne.h"
#include "PlayerRegistration.h"


namespace Algorithm_206038929_314620071 {
    REGISTER_PLAYER(PlayerOne)
    void PlayerOne::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
        processSatelliteView(satellite_view);
        processBattleInfoPreSending();
        tank.updateBattleInfo(battleInfo);
        processBattleInfoPostSending();
    }


    void PlayerOne::updateTankInstructions() {
        if (tankCount == 0)
            battleInfo.setRole(TankRole::Chasing);
        else
            battleInfo.setRole(TankRole::Sniping);
        battleInfo.setRequestInfoCounter(static_cast<int>(tankCount) + 1);
    }

    void PlayerOne::processBattleInfoPostSending() {
        int index = battleInfo.getTankIndex();
        if (!seenAllMyTanks) {
            if (!contains(tankIndices, index)) {
                tankCount++;
                tankIndices.push_back(battleInfo.getTankIndex());
            } else {
                seenAllMyTanks = true;
            }
        } else {
            checkForDeadTanks();
            if (shouldCalculateShells(battleInfo.getTankIndex()))
                battleInfo.assumeEnemyShellsDirections(2);
            removeIfValIsAfter(tankIndicesThatRecentlyDied, index);
        }
    }


    bool PlayerOne::shouldCalculateShells(int tankIndex) {
        if (tankCount == 1)
            return false;
        bool shouldCalculate = !appearsRightAfter(tankIndicesThatRecentlyDied, tankIndex);
        return shouldCalculate;
    }
}