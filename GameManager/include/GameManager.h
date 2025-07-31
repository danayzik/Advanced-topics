#pragma once

#include <array>
#include "ActionRequestUtils.h"
#include "GameMap.h"
#include "AbstractGameManager.h"
#include <sstream>

namespace GameManager_206038929_314620071 {
    using std::string;


    class GameManager : public AbstractGameManager{
    private:
        static constexpr int stepsWithNoAmmoLimit = 40;
        bool verbose = false;
        GameMap gameMap;
        GameResult gameResult = {};
        size_t stepCounter = 0;
        size_t maxSteps = 0;
        size_t numShells = 0;
        bool allTanksOutOfAmmo = false;
        int stepsSinceNoAmmo = 0;
        std::stringstream outputStream = {};
        std::vector<std::string> outputLine = {};
        int totalTankCount = 0;
        std::string mapName = {};
        std::array<Player*, 2> players = {};
        std::array<std::string, 2> playerNames = {};
        std::array<TankAlgorithmFactory , 2> tankAlgorithmFactories = {};
        std::optional<std::unique_ptr<UserCommon_206038929_314620071::ConcreteSatelliteView>> satelliteViewOpt = {};
        std::array<int, 2> tanksPerPlayer = {};
        std::vector<std::unique_ptr<TankAlgorithm>> tankAlgorithms = {};
        std::vector<std::optional<size_t>> tankEntityIds = {};

        void saveGameResult();
        void writeOutputFile();

        void gameLoop();

        [[nodiscard]] bool isLegaLAction(ActionRequest action, const Tank &tank) const;

        void getAndSetAction(Tank &tank);

        void getAndSetActionsForAllTanks();

        void actionStep();

        bool gameOverCheck();

        void shellStep();

        void tankStep();

        void logAction(ActionRequest action);

        void roundTick();

        void registerTank(const Tank &tank);

        void registerAllTanks();

        void checkForDeadTanks();

        void writeOutputLine();



    public:
        GameManager(const GameManager &) = delete;

        GameManager &operator=(const GameManager &) = delete;

        GameManager(GameManager &&) = delete;

        GameManager &operator=(GameManager &&) = delete;

        explicit GameManager(bool verbose): verbose(verbose){}



        GameResult run(
            size_t map_width, size_t map_height,
            const SatelliteView& map,
            string map_name,
            size_t max_steps, size_t num_shells,
            Player& player1, string name1, Player& player2, string name2,
            TankAlgorithmFactory player1_tank_algo_factory,
            TankAlgorithmFactory player2_tank_algo_factory) override;





    };
}