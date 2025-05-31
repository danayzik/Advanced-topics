#pragma once
#include "Player.h"

class PlayerTwo : public Player {
public:
    PlayerTwo( int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells );

    void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) override;
};
