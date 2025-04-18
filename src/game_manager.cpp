#include "../include/game_manager.h"
#include "../include/game_map.h"
#include "../include/entities.h"
#include <thread>
using namespace std::chrono;


GameManager::GameManager(Player &playerOne, Player &playerTwo, const std::string& mapFilePath, bool visuals) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath), visuals(visuals), mapFilePath(mapFilePath){
    auto [tank1, tank2] = gameMap.getPlayerTanks();
    playerOne.setTank(tank1);
    playerTwo.setTank(tank2);
}

//Returns whether the player can perform the requested action
bool GameManager::isLegaLAction(Action action, const Player& player) const{
    Tank* tank = player.getTank();
    TankMode mode = tank->getMode();
    bool preparingReverse = mode == PreparingReverse || mode == JustEnteredReverse;
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

//Signal to the tanks that a round has passed/started for their internal counters.
void GameManager::tanksTickUpdate(){
    playerOne.getTank()->tickUpdate();
    playerTwo.getTank()->tickUpdate();
}

//Requests the action from a player, sets the action their tank will actually perform in this round.
void GameManager::getAndSetAction(Player& player){
    Tank* tank = player.getTank();
    Action action = player.requestAction(gameMap);
    TankMode mode = tank->getMode();
    bool justEnteredReverse = mode == JustEnteredReverse;
    if(justEnteredReverse && action != MoveForward){
        tank->setAction(MoveBackward);
        outputFile << "Player" << player.getPlayerNumber() << " Moving backwards from earlier request"  << '\n';
        return;
    }
    if(isLegaLAction(action, player)){
        bool inReverse = mode == ReverseMode;
        bool preparingReverse = mode == PreparingReverse;
        bool normalMode = (!inReverse) || (!preparingReverse);

        if(action == MoveForward){
            if(inReverse){
                tank->setMode(NormalMode);
            }
            else if(preparingReverse || justEnteredReverse){
                tank->setMode(NormalMode);
                logAction(action, player.getPlayerNumber());
                outputFile << "Player" << player.getPlayerNumber() << " Canceling backwards request"  << '\n';
                tank->setAction(NoAction);
                return;
            }
        }
        if(normalMode && action == MoveBackward){
            tank->setMode(PreparingReverse);
            logAction(action, player.getPlayerNumber());
            tank->setAction(NoAction);
            return;
        }
        logAction(action, player.getPlayerNumber());
        tank->setAction(action);
        return;
    }
    else{
        outputFile << "Player" << player.getPlayerNumber() << " bad step."  << '\n';
        tank->setAction(NoAction);
    }
}

//Performs the action set by "getAndSetAction()". If the action is moving, GameMap moves the tank.
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
    getAndSetAction(playerOne);
    getAndSetAction(playerTwo);
    bool draw = gameMap.tanksAboutToCollide(playerOne.getTank(), playerTwo.getTank());
    if(draw) {
        gameResult = Draw;
        return;
    }
    actionStep(playerOne);
    actionStep(playerTwo);
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

void GameManager::logAction(Action action, int playerNumber) {
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