#include "../include/algorithms.h"
#include <cstdlib>


/*
 * Simplified Algorithm. Shoots if the enemy is in LOS, if not rotates towards his direction,
 * If already facing his direction just move forward.
 * This algorithm often gets stuck on walls, but that's not the point of this exercise.
 */
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
