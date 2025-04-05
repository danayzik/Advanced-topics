#pragma once
#include "cell.h"
#include "direction.h"

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
};

class Tank : public GameEntity{
private:
    enum Direction currDirection = Up;
    int shellCount = 16;
    bool inReverseMode = false;
    bool isAlive = true;
    bool canFire = true;
    int stepsSinceLastShot = 100; // Placeholder initial value
    bool preparingReverseMode = false;

public:
    Tank(int y, int x, Cell *cell);
    inline enum Direction getDirection() { return currDirection; }
    [[nodiscard]] inline int getShellCount() const{ return shellCount; }
    [[nodiscard]] inline bool isInReverse() const { return inReverseMode; }
    [[nodiscard]] inline bool isPreparingReverse() const{ return preparingReverseMode; }
    [[nodiscard]] inline bool getCanFire() const{ return canFire; }
    void rotate(int rotationAmount);
    void fire();
    void putInReverse();
    void moveForward();
    bool collide() override;
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