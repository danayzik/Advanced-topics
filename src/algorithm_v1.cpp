#include "../include/algorithms.h"
#include <cstdlib>



Action AlgorithmV1::getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) {
    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    bool inLineOfSight = enemyInLineOfSight(gameMap, enemyTank, myTank);
    if(inLineOfSight && myTank->canFire()){
        return Shoot;
    }
    Action rotationAction = rotationTowardsEnemy(enemyTank, myTank);
    if(rotationAction != NoAction){
        return rotationAction;
    }
    return MoveForward;

}
