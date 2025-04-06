#include "game_manager.h"
#include "game_map.h"
#include "entities.h"


GameManager::GameManager(Player &playerOne, Player &playerTwo, std::string mapFilePath) : playerOne(playerOne), playerTwo(playerTwo), gameMap(mapFilePath){}


bool GameManager::isLegaLAction(Action action, const Player& player) const{ //Think more
    if(action != MoveForward && player.tank.isPreparingReverse()){
        return false;
    }
    switch (action) {
        case MoveForward:
            if(player.tank.isPreparingReverse()){
                return true;
            }
            return gameMap.tankCanMoveInDirection(&player.tank, player.tank.getDirection());
        case MoveBackward:
            return gameMap.tankCanMoveInDirection(&player.tank, getOppositeDirection(player.tank.getDirection()));
        case RotateLeft45:
            return true;
        case RotateRight45:
            return true;
        case RotateLeft90:
            return true;
        case RotateRight90:
            return true;
        case Shoot:
            return player.tank.getCanFire();
        case NoAction:
            return true;
        default:
            return false;
    }
}

void GameManager::tanksTickUpdate(){
    playerOne.tank.tickUpdate();
    playerTwo.tank.tickUpdate();
}

bool GameManager::getAndSetAction(Player& player){
    Action action = player.requestAction(gameMap);
    if(isLegaLAction(action, playerOne)){
        bool inReverse = player.tank.isInReverse();
        bool preparingReverse = player.tank.isPreparingReverse();
        bool normalMode = (!inReverse) || (!preparingReverse);
        if(action == MoveForward){
            if(inReverse){
                player.tank.cancelReverse();
            }
            if(preparingReverse){
                player.tank.cancelReverse();
                return true;
            }
        }
        else{
            if(player.tank.getEnteredReverse()){
                player.tank.setAction(MoveBackward);
                return true;
            }
        }
        if(normalMode && action == MoveBackward){
            player.tank.putInReverse();
            return true;
        }
        player.tank.setAction(action);
        return true;
    }
    return false;
}

void GameManager::actionStep(Player& player) {
    Action action = player.tank.consumeAction();
    int rotation = rotateActionToAngle(action);
    if(rotation != 0){
        player.tank.rotate(rotation);
    }
    //Need to move tanks at the same time
    if(action == MoveForward){
        //Perform move
    }
    if(action == MoveBackward){
        //Move
    }
    if(action == Shoot){
        //Shoot
    }

}

void GameManager::gameLoop() {
    while(gameRunning){
        tanksTickUpdate();
        getAndSetAction(playerOne);
        getAndSetAction(playerTwo);

        //MoveAll
        //MoveShells

    }


}

void GameManager::run() {
    gameRunning = true;
    gameLoop();
}