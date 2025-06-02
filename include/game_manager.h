#pragma once

#include <array>
#include "actions.h"
#include "game_map.h"
#include "renderer.h"
#include "../common/MyPlayerFactory.h"
#include "../common/MyTankAlgorithmFactory.h"
using std::string;

enum class GameResult {
    WinOccurred,
    DrawFromMaxSteps,
    DrawFromAllTanksDead,
    DrawFromNoAmmo,
    NotOver
};

class GameManager{
private:
    static constexpr int stepsWithNoAmmoLimit = 40;
    static constexpr int stepsPerSecond = 4;

    GameMap gameMap;
    bool visuals = false;
    GameResult gameResult = GameResult::NotOver;

    size_t winningPlayer = 0;

    size_t stepCounter = 0;
    size_t maxSteps;
    size_t numShells;
    bool allTanksOutOfAmmo = false;
    int stepsSinceNoAmmo = 0;
    std::ofstream outputFile;
    std::vector<std::string> outputLine;
    int totalTankCount = 0;
    int playerCount = 0;
    std::optional<unique_ptr<SatelliteView>> satelliteViewOpt = {};
    std::array<int, 9> tanksPerPlayer = {};
    std::array<std::optional<std::unique_ptr<Player>>, 9> players = {};
    std::vector<unique_ptr<TankAlgorithm>> tankAlgorithms = {};
    std::vector<std::optional<size_t>> tankEntityIds = {};
    std::string mapFilePath;
    void gameLoop();
    [[nodiscard]] bool isLegaLAction(ActionRequest action, const Tank& tank) const;
    void getAndSetAction(Tank& tank);
    void getAndSetActionsForAllTanks();
    void actionStep();
    bool gameOverCheck();
    void shellStep();
    void tankStep();
    void logAction(ActionRequest action);
    void roundTick();
    void registerTank(const Tank& tank);
    void registerPlayer(int playerIndex);
    void checkForDeadTanks();
    void writeOutputLine();
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