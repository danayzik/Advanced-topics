#pragma once
#include "Player.h"
#include "FullBattleInfo.h"
#include "PlayerCommunicationModule.h"
class PlayerTwo : public Player, public PlayerCommunicationModule {

private:

    void updateTankInstructions() override;

public:

    PlayerTwo( int player_index,
               size_t x, size_t y,
               size_t max_steps, size_t num_shells ) : Player(player_index, x, y , max_steps, num_shells),
                                                       PlayerCommunicationModule(player_index, x, y, max_steps, num_shells){}
    void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) override;


};
