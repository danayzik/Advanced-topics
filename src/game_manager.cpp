#include "../include/game_manager.h"
#include "../include/game_map.h"
#include "../include/entities.h"
#include <thread>
#include <utility>
using namespace std::chrono;


GameManager::GameManager(Player &playerOne, Player &playerTwo, const std::string& mapFilePath, bool visuals) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath), visuals(visuals), mapFilePath(mapFilePath){
    auto [tank1, tank2] = gameMap.getPlayerTanks();
    playerOne.setTank(tank1);
    playerTwo.setTank(tank2);
}
GameManager::GameManager(MyPlayerFactory playerFactory, MyTankAlgorithmFactory tankAlgorithmFactory) : playerFactory(std::move(playerFactory)),
                                                                                                       tankAlgorithmFactory(std::move(tankAlgorithmFactory)){
}

void GameManager::readBoard(const std::string &mapFile) {
    gameMap.readBoard(mapFile, *this);
    mapFilePath = mapFile;

}

//Returns whether the player can perform the requested action
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
void GameManager::getAndSetAction(int playerIndex, Tank& tank, TankAlgorithm& tankAlgorithm){
    ActionRequest actionRequest = tankAlgorithm.getAction();
    TankMode mode = tank.getMode();
    bool justEnteredReverse = mode == JustEnteredReverse;
    if(justEnteredReverse &&  actionRequest != ActionRequest::MoveForward){
        tank.setAction(ActionRequest::MoveBackward);
        outputFile << "Player" << playerIndex << " Moving backwards from earlier request"  << '\n';
        return;
    }
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
                logAction(actionRequest, playerIndex);
                outputFile << "Player" << playerIndex << " Canceling backwards request"  << '\n';
                tank.setAction(ActionRequest::DoNothing);
                return;
            }
        }
        if(normalMode && actionRequest == ActionRequest::MoveBackward){
            tank.setMode(PreparingReverse);
            logAction(actionRequest, playerIndex);
            tank.setAction(ActionRequest::DoNothing);
            return;
        }
        logAction(actionRequest, playerIndex);
        tank.setAction(actionRequest);
        if(actionRequest == ActionRequest::GetBattleInfo){
            unique_ptr<SatelliteView> view = gameMap.getSatelliteView(tank);
            if(playerIndex == 1){
                playerOne->updateTankWithBattleInfo(tankAlgorithm, *view);
            }
            else{
                playerTwo->updateTankWithBattleInfo(tankAlgorithm, *view);
            }
        }
        return;
    }
    else{
        outputFile << "Player" << playerIndex << " bad step."  << '\n';
        tank.setAction(ActionRequest::DoNothing);
    }
}

//Performs the action set by "getAndSetAction()". If the action is moving, GameMap moves the tank.
void GameManager::actionStep() {
    for(size_t i = 0; i < playerOneTankAlgorithms.size(); i++){
        auto& opt = playerOneTanks.at(i);
        if(!opt){
            //Log as killed for output
            continue;
        }
        if(!gameMap.isTankAlive(opt.value())){
            //log as killed for output
            playerOneTanks[i] = std::nullopt;
        }
        Tank& tank = gameMap.getTank(opt.value());
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
    for(size_t i = 0; i < playerTwoTankAlgorithms.size(); i++){
        auto& opt = playerTwoTanks.at(i);
        if(!opt){
            //Log as killed for output
            continue;
        }
        if(!gameMap.isTankAlive(opt.value())){
            //log as killed for output
            playerTwoTanks[i] = std::nullopt;
        }
        Tank& tank = gameMap.getTank(opt.value());
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
    if(stepsSinceNoAmmo >= 40){
        gameResult = Draw;
        return true;
    }
    gameResult = gameMap.getGameResult();
    if(gameResult != NotOver){
        return true;
    }
    return false;
}


void GameManager::tankStep() {
    for(size_t i = 0; i < playerOneTankAlgorithms.size(); i++){
        TankAlgorithm& algorithm = *(playerOneTankAlgorithms.at(i));
        auto& opt = playerOneTanks.at(i);
        if(!opt){
            //Log as killed for output
            continue;
        }
        if(!gameMap.isTankAlive(opt.value())){
            //log as killed for output
            playerOneTanks[i] = std::nullopt;
        }
        Tank& tank = gameMap.getTank(opt.value());
        tank.tickUpdate();
        getAndSetAction(1, tank, algorithm);
    }
    for(size_t i = 0; i < playerTwoTankAlgorithms.size(); i++){
        TankAlgorithm& algorithm = *(playerTwoTankAlgorithms.at(i));
        auto& opt = playerTwoTanks.at(i);
        if(!opt){
            //Log as killed for output
            continue;
        }
        if(!gameMap.isTankAlive(opt.value())){
            //log as killed for output
            playerTwoTanks[i] = std::nullopt;
        }
        Tank& tank = gameMap.getTank(opt.value());
        tank.tickUpdate();
        getAndSetAction(2, tank, algorithm);
    }
    gameMap.tanksAboutToCollide();
    actionStep();
    gameMap.checkCollisions();
}

//Contains delay only if visuals are on, for viewing experience.
void GameManager::shellStep() {
    const auto stepDuration = milliseconds(1000 / stepsPerSecond);
    auto start = steady_clock::now();
    gameMap.shellsAboutToCollide();
    gameMap.moveShells();
    gameMap.checkCollisions();
    gameMap.updateVisuals();
    auto end = steady_clock::now();
    auto elapsed = duration_cast<milliseconds>(end - start);
    if (elapsed < stepDuration && visuals) {
        std::this_thread::sleep_for(stepDuration - elapsed);
    }
}

void GameManager::roundTick(){
    stepCounter++;
    tanksTickUpdate();
    if(!playerOne.getTank()->hasShells() && !playerTwo.getTank()->hasShells())
        allTanksOutOfAmmo = true;
    if(allTanksOutOfAmmo)
        stepsSinceNoAmmo++;
}

//Main game flow
void GameManager::gameLoop() {
    while(!gameOverCheck()){
        roundTick();
        outputFile << "Step " << stepCounter << '\n';
        tankStep();
        if(gameOverCheck())
            break;
        shellStep();
        if(gameOverCheck())
            break;
        shellStep();

    }
}

void GameManager::logAction(ActionRequest action, int playerNumber) {
    if (outputFile.is_open()) {
        outputFile << "Player" << playerNumber << " " << actionToString(action) << '\n';
    }
}

void GameManager::run() {
    outputFile.open("output_" + mapFilePath);
    gameLoop();
}

GameManager::~GameManager(){
    if (outputFile.is_open()) {
        switch(gameResult){
            case PlayerOneWin:
                outputFile << "Player 1 wins" << '\n';
                break;
            case PlayerTwoWin:
                outputFile << "Player 2 wins" << '\n';
                break;
            case Draw:
                outputFile << "Game ended in a draw" << '\n';
                break;
            default:
                outputFile << "An error has occurred" << '\n';
        }
        outputFile.close();
    }
}

void GameManager::registerTank(int playerIndex, size_t tankEntityIndex) {
    if (playerIndex == 1){
        playerOneTankAlgorithms.push_back(tankAlgorithmFactory.create(playerIndex, player1TankCount));
        playerOneTanks.emplace_back(tankEntityIndex);
        player1TankCount++;
    }
    if(playerIndex == 2){
        playerTwoTankAlgorithms.push_back(tankAlgorithmFactory.create(playerIndex, player2TankCount));
        playerTwoTanks.emplace_back(tankEntityIndex);
        player2TankCount++;
    }
}

