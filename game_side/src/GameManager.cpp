#include "GameManager.h"
#include <thread>
#include <iostream>

using namespace std::chrono;
using namespace DirectionUtils;
using namespace ActionRequestUtils;


GameManager::GameManager(PlayerFactory& playerFactory, TankAlgorithmFactory& tankAlgorithmFactory) : playerFactory(playerFactory),
                                                                                                       tankAlgorithmFactory(tankAlgorithmFactory){
    #ifdef USE_SFML
        visuals = true;
    #endif
}

void GameManager::readBoard(const std::string &mapFile) {
    mapFilePath = mapFile;
    gameMap.readBoard(mapFile, *this);
}

//Returns whether the tank can perform the requested action
bool GameManager::isLegaLAction(ActionRequest action, const Tank& tank) const{
    TankMode mode = tank.getMode();
    bool preparingReverse = mode == PreparingReverse || mode == JustEnteredReverse;
    if(action != ActionRequest::MoveForward && preparingReverse){
        return false;
    }
    switch (action) {
        case ActionRequest::MoveForward:
            if(preparingReverse){
                return true;
            }
            return gameMap.tankCanMoveInDirection(tank, tank.getDirection());
        case ActionRequest::MoveBackward:
            return gameMap.tankCanMoveInDirection(tank, getOppositeDirection(tank.getDirection()));
        case ActionRequest::Shoot:
            return tank.canFire();
        default:
            return true;
    }
}


//Requests the action from a player, sets the action their tank will actually perform in this round.
//If the action is GetBattleInfo, performs it right away
void GameManager::getAndSetAction(Tank& tank){
    TankAlgorithm& tankAlgorithm = *(tankAlgorithms.at(tank.getTankIndex()));
    ActionRequest actionRequest = tankAlgorithm.getAction();
    TankMode mode = tank.getMode();
    int playerIndex = tank.getPlayerIndex();
    bool justEnteredReverse = mode == JustEnteredReverse;
    logAction(actionRequest);
    if(justEnteredReverse &&  actionRequest != ActionRequest::MoveForward){
        tank.setAction(ActionRequest::MoveBackward);
        return;
    }
    //Else ignore? think
    if(isLegaLAction(actionRequest, tank)){
        bool inReverse = mode == ReverseMode;
        bool preparingReverse = mode == PreparingReverse;
        bool normalMode = (!inReverse) || (!preparingReverse);

        if(actionRequest == ActionRequest::MoveForward){
            if(inReverse){
                tank.setMode(NormalMode);
            }
            else if(preparingReverse || justEnteredReverse){
                tank.setMode(NormalMode);
                tank.setAction(ActionRequest::DoNothing);
                return;
            }
        }
        if(normalMode && actionRequest == ActionRequest::MoveBackward){
            tank.setMode(PreparingReverse);
            tank.setAction(ActionRequest::DoNothing);
            return;
        }
        tank.setAction(actionRequest);
        if(actionRequest == ActionRequest::GetBattleInfo){
            if(!satelliteViewOpt){
                satelliteViewOpt = gameMap.getSatelliteView();
            }
            satelliteViewOpt.value()->setRequestingTank(tank);
            (**(players.at(playerIndex-1))).updateTankWithBattleInfo(tankAlgorithm, **satelliteViewOpt);
        }
    }
    else{
        outputLine.back() += " (ignored)";
        tank.setAction(ActionRequest::DoNothing);
    }
}

//Performs the action set by "getAndSetAction()". If the action is moving, GameMap moves the tank.
void GameManager::actionStep() {
    for(size_t i = 0; i < tankAlgorithms.size(); i++){
        auto& tankEntityIDopt = tankEntityIds.at(i);
        if(!tankEntityIDopt){
            continue;
        }
        if(!gameMap.isTankAlive(tankEntityIDopt.value())){
            tankEntityIds[i] = std::nullopt;
            totalTankCount--;
            outputLine[i] += " (killed)";
            continue;
        }
        Tank& tank = gameMap.getTank(tankEntityIDopt.value());
        ActionRequest action = tank.consumeAction();
        int rotation = rotateActionToAngle(action);
        if(rotation != 0){
            tank.rotate(rotation);
            continue;
        }
        switch (action) {
            case ActionRequest::MoveForward:
                gameMap.moveEntity(tank, tank.getDirection());
                break;
            case ActionRequest::MoveBackward:
                gameMap.moveEntity(tank, getOppositeDirection(tank.getDirection()));
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

bool GameManager::gameOverCheck(){
    if(stepsSinceNoAmmo >= stepsWithNoAmmoLimit){
        gameResult = GameResult::DrawFromNoAmmo;
        return true;
    }
    if(totalTankCount == 0){
        gameResult = GameResult::DrawFromAllTanksDead;
        return true;
    }
    if(stepCounter >= maxSteps){
        gameResult = GameResult::DrawFromMaxSteps;
        return true;
    }
    tanksPerPlayer = {};
    for(auto& tankEntityIdOpt : tankEntityIds){
        if(tankEntityIdOpt){
            Tank& tank = gameMap.getTank(tankEntityIdOpt.value());
            tanksPerPlayer[tank.getPlayerIndex()-1]++;
        }
    }
    for(size_t i = 0; i < tanksPerPlayer.size(); i++){
        if(tanksPerPlayer[i] == totalTankCount){
            gameResult = GameResult::WinOccurred;
            winningPlayer = i + 1;
            return true;
        }
    }
    return false;
}


void GameManager::getAndSetActionsForAllTanks() {
    for(size_t i = 0; i < tankAlgorithms.size(); i++){
        auto& tankEntityIdOpt = tankEntityIds.at(i);
        if(!tankEntityIdOpt){
            outputLine.emplace_back("killed");
            continue;
        }
        Tank& tank = gameMap.getTank(tankEntityIdOpt.value());
        getAndSetAction(tank);
    }
}

void GameManager::tankStep() {
    getAndSetActionsForAllTanks();
    gameMap.tanksAboutToCollide();
    actionStep();
    gameMap.checkCollisions();
    checkForDeadTanks();
}

//Contains delay only if visuals are on, for viewing experience.
void GameManager::shellStep() {
    const auto stepDuration = milliseconds(1000 / stepsPerSecond);
    auto start = steady_clock::now();
    gameMap.shellsAboutToCollide();
    gameMap.moveShells();
    gameMap.checkCollisions();
    checkForDeadTanks();
    gameMap.updateVisuals();
    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    if (elapsed < stepDuration && visuals) {
        std::this_thread::sleep_for(stepDuration - elapsed);
    }
}

void GameManager::roundTick(){
    stepCounter++;
    outputLine = {};
    satelliteViewOpt = std::nullopt;
    if(allTanksOutOfAmmo)
        stepsSinceNoAmmo++;
    allTanksOutOfAmmo = true;
    for (auto tankIdOpt : tankEntityIds){
        if(tankIdOpt){
            Tank& tank = gameMap.getTank(tankIdOpt.value());
            tank.tickUpdate();
            if(tank.hasShells()){
                allTanksOutOfAmmo = false;
            }
        }
    }
}

//Main game flow
void GameManager::gameLoop() {
    while(!gameOverCheck()){
        roundTick();
        tankStep();
        if(gameOverCheck())
            break;
        shellStep();
        if(gameOverCheck())
            break;
        shellStep();
        writeOutputLine();
    }
}

void GameManager::logAction(ActionRequest action) {
    outputLine.push_back(actionToString(action));
}

void GameManager::run() {
    outputFile.open("output_" + mapFilePath);
    gameLoop();
}

GameManager::~GameManager(){
    int playersPrinted = 0;
    std::string playerQuantifier = playerCount > 2 ? "all" : "both";
    if (outputFile.is_open()) {
        switch(gameResult){
            case GameResult::DrawFromMaxSteps:
                outputFile << "Tie, reached max steps = " << maxSteps << ", ";
                for(size_t i = 0; i < tanksPerPlayer.size(); i++){
                    if(players[i]){
                        outputFile << "Player " << i + 1 << "has " << tanksPerPlayer[i] << "tanks";
                        playersPrinted++;
                        if(playersPrinted < playerCount){
                            outputFile << ", ";
                        }
                    }
                }
                break;
            case GameResult::DrawFromAllTanksDead:
                outputFile << "Tie, " << playerQuantifier << " players have zero tanks";
                break;
            case GameResult::DrawFromNoAmmo:
                outputFile << "Tie, " << playerQuantifier << " players have zero shells for " << maxSteps << " steps";
                break;
            case GameResult::WinOccurred:
                outputFile << "Player " << winningPlayer << " won with " << totalTankCount << " tanks still alive";
                break;
            default:
                outputFile << "An error has occurred" << '\n';
        }
        outputFile.close();
    }
}

void GameManager::checkForDeadTanks() {
    for(size_t i = 0; i < tankAlgorithms.size(); i++){
        auto& tankEntityIdOpt = tankEntityIds.at(i);
        if(!tankEntityIdOpt){
            continue;
        }
        if(!gameMap.isTankAlive(tankEntityIdOpt.value())){
            tankEntityIds[i] = std::nullopt;
            totalTankCount--;
            outputLine[i] += " (killed)";
        }
    }
}

void GameManager::writeOutputLine() {
    for (size_t i = 0; i < outputLine.size(); ++i) {
        outputFile << outputLine[i];
        if (i + 1 < outputLine.size()) outputFile << ", ";
    }
    outputFile << '\n';
}

void GameManager::registerTank(const Tank& tank) {
    int playerIndex = tank.getPlayerIndex();
    int tankIndex = tank.getTankIndex();
    size_t tankId = tank.getEntityId();
    std::cout << "Registering tank for player: " << playerIndex << std::endl;
    std::cout << "Tank index: " << tankIndex << std::endl;
    std::cout << "Tank entity id: " << tankId << std::endl;
    totalTankCount++;
    tankAlgorithms.push_back(tankAlgorithmFactory.create(playerIndex, tankIndex));
    tankEntityIds.emplace_back(tankId);
}

void GameManager::registerPlayer(int playerIndex) {
    std::cout << "Registering player: " << playerIndex << std::endl;
    playerCount++;
    players[playerIndex-1] = playerFactory.create(playerIndex, gameMap.getCols(), gameMap.getRows(), maxSteps, numShells);
}

