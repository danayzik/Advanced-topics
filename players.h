#pragma once
#include "tank.h"
#include "actions.h"
#include "game_map.h"


class Player {
private:
    Tank tank;

public:
    virtual Action requestAction(GameMap& gameMap) = 0;

};


