#pragma once
#include "entities.h"
#include "actions.h"
#include "game_map.h"

class GameManager;
class Player {
private:
    Tank* tank;

public:
    virtual Action requestAction(const GameMap& gameMap) = 0;
    [[nodiscard]] inline Tank* getTank() const{return tank;}
    inline void setTank(Tank* newTank){tank = newTank;}

};


