#include "entities.h"

bool Wall::collide() {
    health -= 1;
    if(health == 0){
        //Destroy wall
    }
    return true;
}

Wall::Wall(int y, int x, Cell *cell) : GameEntity(y, x ,cell){}