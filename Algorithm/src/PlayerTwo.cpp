#include "PlayerTwo.h"

namespace Algorithm_206038929_314620071 {
    void PlayerTwo::updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) {
        processSatelliteView(satellite_view);
        processBattleInfoPreSending();
        tank.updateBattleInfo(battleInfo);
        processBattleInfoPostSending();
    }


    void PlayerTwo::updateTankInstructions() {
        if (tankCount <= 1)
            battleInfo.setRole(TankRole::Sniping);
        else
            battleInfo.setRole(TankRole::Chasing);
        battleInfo.setRequestInfoCounter(4);
    }

}