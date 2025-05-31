#include "../include/algorithms.h"
#include <cstdlib>


/*
 * Simplified Algorithm. Shoots if the enemy is in LOS, if tank can move and chased by shell will move,
 * This algorithm often gets stuck on walls, but that's not the point of this exercise.
 */
ActionRequest AlgorithmV1::getAction(const GameMap& gameMap, int playerNumber, const Tank* myTank) {


    const Tank* enemyTank = gameMap.getEnemyTank(playerNumber);
    bool inLineOfSight = enemyInLineOfSight(gameMap, enemyTank, myTank);
    if(inLineOfSight && myTank->canFire()){
        return Shoot;
    }

    const auto& grid = gameMap.getGrid();
    int cols = gameMap.getCols();
    int rows = gameMap.getRows();
    auto [dy, dx] = directionToCoordinatesOffset(myTank->getDirection());
    int newY = (myTank->getY() + dy + rows) % rows;
    int newX = (myTank->getX() + dx + cols) % cols;
    bool canMove = grid[newY][newX].isPassableForTank();
    bool chasedByShell = hasShellMovingTowardsTank(gameMap, myTank);

    if(canMove && chasedByShell)
        return MoveForward;

    ActionRequest rotationAction = rotationTowardsEnemy(enemyTank, myTank);
    if(rotationAction != DoNothing){
        return rotationAction;
    }
    if(canMove)
        return MoveForward;
    return RotateRight45;

}
