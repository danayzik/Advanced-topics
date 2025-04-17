#pragma once
#include "entities.h"
#include "actions.h"
#include "game_map.h"
#include "algorithms.h"
#include <memory>




class GameManager;
class Player {

protected:
    int playerNumber;
    Tank* tank = nullptr;

private:
    std::unique_ptr<Algorithm> algorithm;


public:
    virtual inline Action requestAction(const GameMap& gameMap){return algorithm->getAction(gameMap, playerNumber, tank);}
    inline Player(int number, std::unique_ptr<Algorithm> algo) : playerNumber(number), algorithm(std::move(algo)){}
    [[nodiscard]] inline Tank* getTank() const{return tank;}
    inline void setTank(Tank* newTank){tank = newTank;}
    [[nodiscard]] inline int getPlayerNumber() const{return playerNumber;}
};


