#pragma once
#include "Player.h"
#include "FullBattleInfo.h"
#include "PlayerCommunicationModule.h"
namespace Algorithm_206038929_314620071 {
    class PlayerOne : public Player, public PlayerCommunicationModule {

    private:

        bool shouldCalculateShells(int tankIndex);

    public:

        PlayerOne(int player_index,
                  size_t x, size_t y,
                  size_t max_steps, size_t num_shells) :PlayerCommunicationModule(player_index, x, y, max_steps, num_shells) {}

        void updateTankInstructions() override;

        void updateTankWithBattleInfo(TankAlgorithm &tank, SatelliteView &satellite_view) override;

        void processBattleInfoPostSending() override;


    };
}
