#pragma once
#include "players.h"
#include "actions.h"
#include "game_map.h"
#include "renderer.h"
using std::string;

class GameManager{
private:

    std::ofstream outputFile;
    Player& playerOne;
    Player& playerTwo;
    GameMap gameMap;
    GameResult gameResult = NotOver;
    int stepCounter = 1;
    bool gameRunning = false;
    bool visuals;
    int stepsPerSecond = 2;
    void gameLoop();
    [[nodiscard]] bool isLegaLAction(Action action, const Player& player) const;
    bool getAndSetAction(Player& player);
    void tanksTickUpdate();
    void actionStep(Player& player);
    bool gameOverCheck();
    void shellStep();
    void tankStep();
    void logAction(Action action, int playerNumber);

public:
    GameManager(Player& playerOne, Player& playerTwo, const string& mapFilePath, bool visuals = false);
    GameResult run();
    ~GameManager();


};