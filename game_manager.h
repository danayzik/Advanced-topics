#pragma once
#include "players.h"
#include "actions.h"
#include "game_map.h"
using std::string;

class GameManager{
private:
    Player& playerOne;
    Player& playerTwo;
    GameMap gameMap;
    void gameLoop();
    bool gameRunning = false;
    bool isLegaLAction(Action action, const Player& player) const;
    bool getAndSetAction(Player& player);
    void tanksTickUpdate();
    void actionStep(Player& player);

public:
    GameManager(Player& playerOne, Player& playerTwo, string mapFilePath);
    void run();
    bool setUpMatch();


};