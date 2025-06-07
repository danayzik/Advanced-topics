#pragma once
#include "TankAlgorithm.h"
#include "SatelliteView.h"

class Player {
public:
    Player( int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells ) {}
    virtual ~Player() = default;
    virtual void updateTankWithBattleInfo
            (TankAlgorithm& tank, SatelliteView& satellite_view) = 0;
};
