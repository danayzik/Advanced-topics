#include "game_manager.h"
#include "game_map.h"
#include "entities.h"


GameManager::GameManager(Player &playerOne, Player &playerTwo, const std::string& mapFilePath) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath){
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
    if(isLegaLAction(action, playerOne)){
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
        tank->setAction(action);
        return true;
    }
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
    gameMap.shellsAboutToCollide();
    gameMap.moveShells();
    gameMap.checkCollisions();
}

void GameManager::gameLoop() {
    while(!gameOverCheck()){
        tankStep();
        if(gameOverCheck())
            break;
        shellStep();
        if(gameOverCheck())
            break;
        shellStep();

    }
}


GameResult GameManager::run() {
    gameRunning = true;
    gameLoop();
    return gameResult;
}