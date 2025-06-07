#pragma once
#include "Player.h"
#include "FullBattleInfo.h"
class PlayerOne : public Player {

private:
    int playerIndex;
    size_t rows;
    size_t cols;
    size_t maxSteps;
    size_t numShells;
    FullBattleInfo battleInfo;


public:

    PlayerOne( int player_index,
            size_t x, size_t y,
            size_t max_steps, size_t num_shells ) : Player(player_index, x, y , max_steps, num_shells), playerIndex(player_index), rows(y), cols(x), maxSteps(max_steps), numShells(num_shells), battleInfo(rows, cols, numShells){}


    void updateTankWithBattleInfo(TankAlgorithm& tank, SatelliteView& satellite_view) override;


};
