#pragma once
#include "entities.h"
#include "actions.h"
#include "game_map.h"

struct CellSearchItem{
    int y;
    int x;
    int distanceFromTarget;
    CellSearchItem(int y, int x , int distance): y(y), x(x), distanceFromTarget(distance){}
    CellSearchItem() = default;
};

class GameManager;
class Player {

protected:
    int playerNumber;
    Tank* tank = nullptr;
public:
    virtual inline Action requestAction(const GameMap& gameMap){return NoAction;}
    inline explicit Player(int number) : playerNumber(number){}
    [[nodiscard]] inline Tank* getTank() const{return tank;}
    inline void setTank(Tank* newTank){tank = newTank;}
};


class ChasingPlayer : public Player{

public:
    Action requestAction(const GameMap& gameMap) override;
    inline explicit ChasingPlayer(int num): Player(num){}

};

class PlayerV1 : public Player{

private:
    [[nodiscard]] int distanceToEnemy(const GameMap& gameMap) const;
    [[nodiscard]] bool enemyInLineOfSight(const GameMap& gameMap) const;
public:
    Action requestAction(const GameMap& gameMap) override;
    inline explicit PlayerV1(int num): Player(num){}


};


