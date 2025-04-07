#pragma once
#include "cell.h"
#include "direction.h"
#include "actions.h"

enum TankMode {
    NormalMode,
    PreparingReverse,
    ReverseMode
};

class GameEntity{
protected:
    Cell* cell;
    int xCoord;
    int yCoord;

public:
    inline virtual ~GameEntity(){
        cell->entitySet.erase(this);
    }
    inline GameEntity(int y, int x, Cell* cell) : xCoord(x), yCoord(y), cell(cell){}
    [[nodiscard]] inline Cell* getCell() const{
        return cell;
    }
    inline void setCell(Cell* newCell){
        cell = newCell;
    }

    inline void setCoords(int x, int y){
        xCoord = x;
        yCoord = y;
    }
    [[nodiscard]] inline int getX() const{
        return xCoord;
    }
    [[nodiscard]] inline int getY() const{
        return yCoord;
    }
    virtual bool collide() = 0;
    inline virtual bool isMine(){return false;}
    inline virtual bool isWall(){return true;}
};

class Tank : public GameEntity{
private:
    enum Direction currDirection = Up;
    int shellCount = 16;
    bool isAlive = true;
    bool canFire = true;
    int stepsSinceLastShot = 100; // Placeholder initial value
    int preparingReverseCounter = -1;
    Action nextStepAction = NoAction;
    bool enteredReverse = false;
    TankMode mode = NormalMode;


public:
    Tank(int y, int x, Cell *cell);
    [[nodiscard]] inline enum Direction getDirection() const { return currDirection; }
    [[nodiscard]] inline int getShellCount() const{ return shellCount; }
    [[nodiscard]] inline bool getCanFire() const{ return canFire; }
    [[nodiscard]] inline bool getEnteredReverse() const{ return enteredReverse; }
    void rotate(int rotationAmount);
    void setMode(TankMode newMode);
    [[nodiscard]] inline TankMode getMode() const {return mode;}
    void fire();
    void moveForward();
    bool collide() override;
    Action consumeAction();
    void setAction(Action action);
    void tickUpdate();


};

class Shell : public GameEntity{
private:
    const enum Direction dir;
public:
    Shell(int y, int x, Cell *cell, Direction dir);
    [[nodiscard]] inline enum Direction getDirection() const{
        return dir;
    }
};

class Wall : public GameEntity{
private:
    int health = 2;

public:
    Wall(int y, int x, Cell *cell);
    bool collide() override;
    inline bool isWall() override{
        return true;
    }
};

class Mine : public GameEntity{
public:
    Mine(int y, int x, Cell *cell);
    inline bool collide() override{
        return false;
    }
    inline bool isMine() override{
        return true;
    }
};