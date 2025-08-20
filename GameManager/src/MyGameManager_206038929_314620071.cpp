#include "MyGameManager_206038929_314620071.h"
#include <iostream>
#include <utility>
#include "GameManagerRegistration.h"
#include <cassert>
#include <filesystem>


namespace GameManager_206038929_314620071 {
    REGISTER_GAME_MANAGER(MyGameManager_206038929_314620071)


    using namespace ActionRequestUtils;



//Returns whether the tank can perform the requested action
    bool MyGameManager_206038929_314620071::isLegaLAction(ActionRequest action, const Tank &tank) const {
        TankMode mode = tank.getMode();
        bool preparingReverse = mode == PreparingReverse || mode == JustEnteredReverse;
        if (action != ActionRequest::MoveForward && preparingReverse) {
            return false;
        }
        switch (action) {
            case ActionRequest::MoveForward:
                if (preparingReverse) {
                    return true;
                }
                return gameMap.tankCanMoveInDirection(tank, tank.getDirection());
            case ActionRequest::MoveBackward:
                return gameMap.tankCanMoveInDirection(tank, DirectionUtils::getOppositeDirection(tank.getDirection()));
            case ActionRequest::Shoot:
                return tank.canFire();
            default:
                return true;
        }
    }


//Requests the action from a tank, sets the action the tank will actually perform in this round.
//If the action is GetBattleInfo, performs it right away
    void MyGameManager_206038929_314620071::getAndSetAction(Tank &tank) {
        TankAlgorithm &tankAlgorithm = *(tankAlgorithms.at(tank.getTankIndex()));
        ActionRequest actionRequest;
        try{
            actionRequest = tankAlgorithm.getAction();
        }
        catch (...){
            actionRequest = ActionRequest::DoNothing;
        }

        TankMode mode = tank.getMode();
        int playerIndex = tank.getPlayerIndex();
        bool justEnteredReverse = mode == JustEnteredReverse;
        logAction(actionRequest);
        bool notMoveForward = actionRequest != ActionRequest::MoveForward;
        if (justEnteredReverse && notMoveForward) {
            tank.setAction(ActionRequest::MoveBackward);
            outputLine.back() += " (ignored)";
            return;
        }
        if (isLegaLAction(actionRequest, tank)) {
            bool inReverse = mode == ReverseMode;
            bool preparingReverse = mode == PreparingReverse;
            bool normalMode = mode == NormalMode;
            if (inReverse && actionRequest != ActionRequest::MoveBackward) {
                tank.setMode(NormalMode);
            }
            if (actionRequest == ActionRequest::MoveForward) {
                if (preparingReverse || justEnteredReverse) {
                    tank.setMode(NormalMode);
                    tank.setAction(ActionRequest::DoNothing);
                    return;
                }
            }
            if (normalMode && actionRequest == ActionRequest::MoveBackward) {
                tank.setMode(PreparingReverse);
                tank.setAction(ActionRequest::DoNothing);
                return;
            }
            tank.setAction(actionRequest);
            if (actionRequest == ActionRequest::GetBattleInfo) {
                if (!satelliteViewOpt) {
                    satelliteViewOpt = gameMap.getSatelliteView();
                }
                satelliteViewOpt.value()->setRequestingTank(tank.getCoords(), tank.getSymbol());
                players[playerIndex-1]->updateTankWithBattleInfo(tankAlgorithm, **satelliteViewOpt);
            }
        } else {
            outputLine.back() += " (ignored)";
            tank.setAction(ActionRequest::DoNothing);
        }
    }

//Performs the action set by "getAndSetAction()". If the action is moving, GameMap moves the tank.
    void MyGameManager_206038929_314620071::actionStep() {
        for (size_t i = 0; i < tankAlgorithms.size(); i++) {
            auto &tankEntityIDopt = tankEntityIds.at(i);
            if (!tankEntityIDopt) {
                continue;
            }
            if (!gameMap.isTankAlive(tankEntityIDopt.value())) {
                tankEntityIds[i] = std::nullopt;
                totalTankCount--;
                outputLine[i] += " (killed)";
                continue;
            }
            Tank &tank = gameMap.getTank(tankEntityIDopt.value());
            ActionRequest action = tank.consumeAction();
            int rotation = rotateActionToAngle(action);
            if (rotation != 0) {
                tank.rotate(rotation);
                continue;
            }
            switch (action) {
                case ActionRequest::MoveForward:
                    gameMap.moveEntity(tank, tank.getDirection());
                    break;
                case ActionRequest::MoveBackward:
                    gameMap.moveEntity(tank, DirectionUtils::getOppositeDirection(tank.getDirection()));
                    break;
                case ActionRequest::Shoot:
                    tank.fire();
                    gameMap.fireShell(tank);
                    break;
                default:
                    break;
            }
        }

    }

    bool MyGameManager_206038929_314620071::gameOverCheck() {
        tanksPerPlayer = {};
        if (totalTankCount == 0) {
            gameResult.reason = GameResult::Reason::ALL_TANKS_DEAD;
            gameResult.winner = 0;
            return true;
        }
        if (stepsSinceNoAmmo > stepsWithNoAmmoLimit) {
            gameResult.reason = GameResult::Reason::ZERO_SHELLS;
            gameResult.winner = 0;
            return true;
        }
        if (stepCounter > maxSteps) {
            gameResult.reason = GameResult::Reason::MAX_STEPS;
            gameResult.winner = 0;
            return true;
        }
        for (auto &tankEntityIdOpt: tankEntityIds) {
            if (tankEntityIdOpt) {
                Tank &tank = gameMap.getTank(tankEntityIdOpt.value());
                tanksPerPlayer[tank.getPlayerIndex() - 1]++;
            }
        }
        for (size_t i = 0; i < tanksPerPlayer.size(); i++) {
            if (tanksPerPlayer[i] == totalTankCount) {
                gameResult.reason = GameResult::Reason::ALL_TANKS_DEAD;
                gameResult.winner = static_cast<int>(i) + 1;
                return true;
            }
        }

        return false;
    }


    void MyGameManager_206038929_314620071::getAndSetActionsForAllTanks() {
        for (size_t i = 0; i < tankAlgorithms.size(); i++) {
            auto &tankEntityIdOpt = tankEntityIds.at(i);
            if (!tankEntityIdOpt) {
                outputLine.emplace_back("killed");
                continue;
            }
            Tank &tank = gameMap.getTank(tankEntityIdOpt.value());
            getAndSetAction(tank);
        }
    }

    void MyGameManager_206038929_314620071::tankStep() {
        getAndSetActionsForAllTanks();
        gameMap.tanksAboutToCollide();
        actionStep();
        gameMap.checkCollisions();
        checkForDeadTanks();
    }

//Contains delay only if visuals are on, for viewing experience.
    void MyGameManager_206038929_314620071::shellStep() {
        gameMap.shellsAboutToCollide();
        gameMap.moveShells();
        gameMap.checkCollisions();
        checkForDeadTanks();
    }

    void MyGameManager_206038929_314620071::roundTick() {
        stepCounter++;
        satelliteViewOpt = std::nullopt;
        allTanksOutOfAmmo = true;
        for (auto tankIdOpt: tankEntityIds) {
            if (tankIdOpt) {
                Tank &tank = gameMap.getTank(tankIdOpt.value());
                tank.tickUpdate();
                if (tank.hasShells()) {
                    allTanksOutOfAmmo = false;
                }
            }
        }
        if (allTanksOutOfAmmo)
            stepsSinceNoAmmo++;
    }

//Main game flow
    void MyGameManager_206038929_314620071::gameLoop() {
        while (!gameOverCheck()) {
            roundTick();
            if (gameOverCheck()) {
                stepCounter--;
                break;
            }
            tankStep();
            if (gameOverCheck()) {
                break;
            }
            shellStep();
            if (gameOverCheck()) {
                break;
            }
            shellStep();
            writeOutputLine();
        }
    }

    void MyGameManager_206038929_314620071::saveGameResult() {
        gameResult.gameState = gameMap.getSatelliteView();
        for (auto count : tanksPerPlayer) {
            assert(count >= 0);
            gameResult.remaining_tanks.push_back(static_cast<size_t>(count));
        }
        gameResult.rounds = stepCounter;


    }

    void MyGameManager_206038929_314620071::logAction(ActionRequest action) {
        outputLine.push_back(actionToString(action));
    }

    GameResult MyGameManager_206038929_314620071::run(size_t map_width, size_t map_height,
                                                      const SatelliteView& map,
                                                      string map_name,
                                                      size_t max_steps, size_t num_shells,
                                                      Player& player1, string name1, Player& player2, string name2,
                                                      TankAlgorithmFactory player1_tank_algo_factory,
                                                      TankAlgorithmFactory player2_tank_algo_factory) {
        gameResult = {};
        tankAlgorithmFactories[0] = std::move(player1_tank_algo_factory);
        tankAlgorithmFactories[1] = std::move(player2_tank_algo_factory);
        std::filesystem::path p1(name1);
        std::filesystem::path p2(name2);
        playerNames[0] = p1.stem().string();
        playerNames[1] = p2.stem().string();
        players[0] = &player1;
        players[1] = &player2;
        maxSteps = max_steps;
        numShells = num_shells;
        gameMap.buildMap(map, map_height, map_width, numShells);
        mapName = std::move(map_name);
        registerAllTanks();
        gameLoop();
        saveGameResult();
        if(verbose)
            writeOutputFile();
        cleanUp();
        return std::move(gameResult);
    }


    void MyGameManager_206038929_314620071::writeOutputFile() {
        std::ofstream outputFile = {};

        outputFile.open("Manager_206038929_314620071_Players_" + playerNames[0] + "_" + playerNames[1] + "_map_" + mapName);
        if (!outputLine.empty())
            writeOutputLine();
        if(gameResult.winner != 0){
            outputStream << "Player " << gameResult.winner << " won with " << totalTankCount << " tanks still alive";
            outputFile << outputStream.str();
            outputFile.close();
            return;
        }

        switch (gameResult.reason) {
            case GameResult::Reason::MAX_STEPS:
                outputStream << "Tie, reached max steps = " << maxSteps << ", ";
                for (size_t i = 0; i < players.size(); i++) {
                    outputStream << "Player " << i + 1 << " has " << tanksPerPlayer[i] << " tanks";
                    if (i < players.size()-1) {
                        outputStream << ", ";
                    }
                }
                break;
            case GameResult::Reason::ALL_TANKS_DEAD:
                outputStream << "Tie, both players have zero tanks";
                break;
            case GameResult::Reason::ZERO_SHELLS:
                outputStream << "Tie, both" << " players have zero shells for "
                             << stepsWithNoAmmoLimit << " steps";

        }
        outputFile << outputStream.str();
        outputFile.close();

    }


    void MyGameManager_206038929_314620071::checkForDeadTanks() {
        for (size_t i = 0; i < tankAlgorithms.size(); i++) {
            auto &tankEntityIdOpt = tankEntityIds.at(i);
            if (!tankEntityIdOpt) {
                continue;
            }
            if (!gameMap.isTankAlive(tankEntityIdOpt.value())) {
                tankEntityIds[i] = std::nullopt;
                totalTankCount--;
                outputLine[i] += " (killed)";
            }
        }
    }

    void MyGameManager_206038929_314620071::writeOutputLine() {
        for (size_t i = 0; i < outputLine.size(); ++i) {
            outputStream << outputLine[i];
            if (i + 1 < outputLine.size()) outputStream << ", ";
        }
        outputStream << '\n';
        outputLine = {};
    }

    void MyGameManager_206038929_314620071::registerTank(const Tank &tank) {
        int playerIndex = tank.getPlayerIndex();
        int tankIndex = tank.getTankIndex();
        size_t tankId = tank.getEntityId();
        totalTankCount++;
        tankAlgorithms.push_back(tankAlgorithmFactories[playerIndex-1](playerIndex, tankIndex));
        tankEntityIds.emplace_back(tankId);
    }

    void MyGameManager_206038929_314620071::registerAllTanks() {
        auto tankIds = gameMap.stealTempTankIds();
        for(auto tankId : tankIds){
            const Tank& tank = gameMap.getTank(tankId);
            registerTank(tank);
        }
    }

    void MyGameManager_206038929_314620071::cleanUp() {
        stepCounter = 0;
        maxSteps = 0;
        numShells = 0;
        allTanksOutOfAmmo = false;
        stepsSinceNoAmmo = 0;
        outputStream = {};
        outputLine = {};
        totalTankCount = 0;
        mapName = {};
        players = {};
        playerNames = {};
        tankAlgorithmFactories = {};
        satelliteViewOpt = {};
        tanksPerPlayer = {};
        tankAlgorithms.clear();
        tankEntityIds.clear();
        tankAlgorithms.shrink_to_fit();
        tankEntityIds.shrink_to_fit();
        gameMap.cleanUp();
    }
}