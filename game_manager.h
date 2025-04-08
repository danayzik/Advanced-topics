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
    GameResult gameResult = NotOver;
    bool gameRunning;
    void gameLoop();
    bool isLegaLAction(Action action, const Player& player) const;
    bool getAndSetAction(Player& player);
    void tanksTickUpdate();
    void actionStep(Player& player);
    bool gameOverCheck();
    void shellStep();
    void tankStep();

public:
    GameManager(Player& playerOne, Player& playerTwo, string mapFilePath);
    GameResult run();

};