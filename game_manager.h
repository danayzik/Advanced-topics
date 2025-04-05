#pragma once
#include "players.h"
#include "actions.h"
using std::string;

class GameManager{
private:
    Player& playerOne;
    Player& playerTwo;

public:
    GameManager(Player& playerOne, Player& playerTwo, string mapFilePath);
    void run();


};