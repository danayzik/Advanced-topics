#pragma once
#include "players.h"
#include "actions.h"
#include "game_map.h"
#include "renderer.h"
using std::string;

class GameManager{
private:
    Player& playerOne;
    Player& playerTwo;
    GameMap gameMap;
    Renderer* renderer;
    GameResult gameResult = NotOver;
    bool gameRunning = false;
    void gameLoop();
    bool isLegaLAction(Action action, const Player& player) const;
    bool getAndSetAction(Player& player);
    void tanksTickUpdate();
    void actionStep(Player& player);
    bool gameOverCheck();
    void shellStep();
    void tankStep();

public:
    GameManager(Player& playerOne, Player& playerTwo, const string& mapFilePath);
    GameResult run();

};