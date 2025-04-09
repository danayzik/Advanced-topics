#pragma once
#include "entities.h"
#include "actions.h"
#include "game_map.h"

struct CellSearchItem{
    int y;
    int x;
    int distanceFromTarget;
};

class GameManager;
class Player {
private:
    Tank* tank = nullptr;

protected:
    int playerNumber;
public:
    virtual Action requestAction(const GameMap& gameMap) = 0;
    [[nodiscard]] inline Tank* getTank() const{return tank;}
    inline void setTank(Tank* newTank){tank = newTank;}

};


class ChasingPlayer : public Player{
private:

public:
    Action requestAction(const GameMap& gameMap) override;


};


