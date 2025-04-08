#include "game_manager.h"
#include "game_map.h"
#include "entities.h"


GameManager::GameManager(Player &playerOne, Player &playerTwo, std::string mapFilePath) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath){}


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
        case RotateLeft45:
            return true;
        case RotateRight45:
            return true;
        case RotateLeft90:
            return true;
        case RotateRight90:
            return true;
        case Shoot:
            return tank->canFire();
        case NoAction:
            return true;
        default:
            return false;
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
            gameMap.createShell(tank->getY(), tank->getX(), tank->getDirection());
            return;
        default:
            return;
    }
}


void GameManager::gameLoop() {
    while(gameRunning){
        tanksTickUpdate();
        getAndSetAction(playerOne);
        getAndSetAction(playerTwo);
        //Check about to collide tanks
        actionStep(playerOne);
        actionStep(playerTwo);
        gameMap.checkCollisions();

        //Check about to collide shells
        gameMap.moveShells();
        gameMap.checkCollisions();

        //Check about to collide shells
        gameMap.moveShells();
        gameMap.checkCollisions();

    }


}

void GameManager::run() {
    gameRunning = true;
    gameLoop();
}