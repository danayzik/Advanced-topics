#include "../include/game_manager.h"
#include "../include/game_map.h"
#include "../include/entities.h"
#include "../include/sfml_renderer.h"
#include <thread>
using namespace std::chrono;
GameManager::GameManager(Player &playerOne, Player &playerTwo, const std::string& mapFilePath, bool visuals) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath, visuals), visuals(visuals){
    auto [tank1, tank2] = gameMap.getPlayerTanks();
    playerOne.setTank(tank1);
    playerTwo.setTank(tank2);
}


bool GameManager::isLegaLAction(Action action, const Player& player) const{
    Tank* tank = player.getTank();
    TankMode mode = tank->getMode();
    bool preparingReverse = mode == PreparingReverse;
    if(action != MoveForward && preparingReverse){
        return false;
    }
    switch (action) {
        case MoveForward:
            if(preparingReverse){
                return true;
            }
            return gameMap.tankCanMoveInDirection(tank, tank->getDirection());
        case MoveBackward:
            return gameMap.tankCanMoveInDirection(tank, getOppositeDirection(tank->getDirection()));
        case Shoot:
            return tank->canFire();
        default:
            return true;
    }
}

void GameManager::tanksTickUpdate(){
    playerOne.getTank()->tickUpdate();
    playerTwo.getTank()->tickUpdate();
}


bool GameManager::getAndSetAction(Player& player){ // Check
    Tank* tank = player.getTank();
    Action action = player.requestAction(gameMap);
    TankMode mode = tank->getMode();
    if(isLegaLAction(action, player)){
        bool inReverse = mode == ReverseMode;
        bool preparingReverse = mode == PreparingReverse;
        bool justEnteredReverse = mode == JustEnteredReverse;
        bool normalMode = (!inReverse) || (!preparingReverse);
        if(action == MoveForward){
            if(inReverse){
                tank->setMode(NormalMode);
            }
            if(preparingReverse || justEnteredReverse){
                tank->setMode(NormalMode);
                return true;
            }
        }
        else{
            if(justEnteredReverse){
                tank->setAction(MoveBackward);
                return true;
            }
        }
        if(normalMode && action == MoveBackward){
            tank->setMode(ReverseMode);
            return true;
        }
        logAction(action, player.getPlayerNumber());
        tank->setAction(action);
        return true;
    }
    outputFile << "Player" << player.getPlayerNumber() << " bad step."  << '\n';
    return false;
}

void GameManager::actionStep(Player& player) {
    Tank* tank = player.getTank();
    Action action = tank->consumeAction();
    int rotation = rotateActionToAngle(action);

    if(rotation != 0){
        tank->rotate(rotation);
        return;
    }
    switch (action) {
        case MoveForward:
            gameMap.moveEntity(tank, tank->getDirection());
            return;
        case MoveBackward:
            gameMap.moveEntity(tank, getOppositeDirection(tank->getDirection()));
            return;
        case Shoot:
            tank->fire();
            gameMap.createShell(tank);
            return;
        default:
            return;
    }
}

bool GameManager::gameOverCheck(){
    gameResult = gameMap.getGameResult();
    if(gameResult != NotOver){
        gameRunning = false;
        return true;
    }
    return false;
}

void GameManager::tankStep() {
    tanksTickUpdate();
    getAndSetAction(playerOne);
    getAndSetAction(playerTwo);
    bool draw = gameMap.tanksAboutToCollide(playerOne.getTank(), playerTwo.getTank());
    if(draw) {
        gameResult = Draw;
        gameRunning = false;
        return;
    }
    actionStep(playerOne);
    actionStep(playerTwo);
    gameMap.checkCollisions();
}

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

void GameManager::gameLoop() {
    while(!gameOverCheck()){
        outputFile << "Step " << stepCounter << '\n';
        tankStep();
        if(gameOverCheck())
            break;
        shellStep();
        if(gameOverCheck())
            break;
        shellStep();
        stepCounter++;
    }
}

void GameManager::logAction(Action action, int playerNumber) {
    if (outputFile.is_open()) {
        outputFile << "Player" << playerNumber << " " << actionToString(action) << '\n';
    }
}

GameResult GameManager::run() {
    outputFile.open("output.txt");
    gameRunning = true;
    gameLoop();
    return gameResult;
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