#pragma once
#include "actions.h"
#include "game_map.h"
#include "renderer.h"
#include "../common/MyPlayerFactory.h"
#include "../common/MyTankAlgorithmFactory.h"
using std::string;

class GameManager{
private:
    int player1TankCount = 0;
    int player2TankCount = 0;
    unique_ptr<Player> playerOne;
    unique_ptr<Player> playerTwo;
    GameMap gameMap;
    bool visuals = false;
    GameResult gameResult = NotOver;
    size_t stepCounter = 0;
    size_t maxSteps;
    size_t numShells;
    bool allTanksOutOfAmmo = false;
    int stepsSinceNoAmmo = 0;
    std::ofstream outputFile;
    int stepsPerSecond = 4;
    std::vector<unique_ptr<TankAlgorithm>> playerOneTankAlgorithms = {};
    std::vector<unique_ptr<TankAlgorithm>> playerTwoTankAlgorithms = {};
    std::vector<std::optional<size_t> > playerOneTanks = {};
    std::vector<std::optional<size_t> > playerTwoTanks = {};
    std::string mapFilePath;
    void gameLoop();
    [[nodiscard]] bool isLegaLAction(ActionRequest action, const Tank& tank) const;
    void getAndSetAction(int playerIndex, Tank& tank, TankAlgorithm& tankAlgorithm);
    void actionStep();
    bool gameOverCheck();
    void shellStep();
    void tankStep();
    void logAction(ActionRequest action, int playerNumber);
    void roundTick();
    void registerTank(int playerIndex, size_t tankEntityIndex);
    MyPlayerFactory playerFactory;
    MyTankAlgorithmFactory tankAlgorithmFactory;

public:
    GameManager(MyPlayerFactory playerFactory, MyTankAlgorithmFactory tankAlgorithmFactory);
    void readBoard(const string& mapFile);
    GameManager(Player& playerOne, Player& playerTwo, const string& mapFilePath, bool visuals = false);
    void run();
    ~GameManager();
    friend MapLoader;



};