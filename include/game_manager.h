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
    bool visuals;
    const string& mapFilePath;
    GameResult gameResult = NotOver;
    int stepCounter = 0;
    bool allTanksOutOfAmmo = false;
    int stepsSinceNoAmmo = 0;
    std::ofstream outputFile;
    int stepsPerSecond = 4;
    void gameLoop();
    [[nodiscard]] bool isLegaLAction(Action action, const Player& player) const;
    void getAndSetAction(Player& player);
    void tanksTickUpdate();
    void actionStep(Player& player);
    bool gameOverCheck();
    void shellStep();
    void tankStep();
    void logAction(Action action, int playerNumber);
    void roundTick();

public:
    GameManager(Player& playerOne, Player& playerTwo, const string& mapFilePath, bool visuals = false);
    void run();
    ~GameManager();


};