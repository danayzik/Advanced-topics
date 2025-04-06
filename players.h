#pragma once
#include "entities.h"

#include "actions.h"
#include "game_map.h"

class GameManager;
class Player {
private:
    Tank tank;

public:
    virtual Action requestAction(const GameMap& gameMap) = 0;
    friend GameManager;

};


