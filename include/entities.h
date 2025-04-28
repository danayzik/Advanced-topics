#pragma once
#include "cell.h"
#include "direction.h"
#include "actions.h"

enum TankMode {
    NormalMode,
    PreparingReverse,
    JustEnteredReverse,
    ReverseMode
};
enum Owner{
    Game,
    PlayerOne,
    PlayerTwo
};

class GameEntity{

protected:


    int yCoord;
    int xCoord;
    Cell* cell; //Does not own
    int health;
    Owner owner = Game;

public:
    inline virtual ~GameEntity(){
        cell->entitySet.erase(this);
    }

    inline void setOwner(Owner newOwner){owner = newOwner;}
    [[nodiscard]] inline Owner getOwner() const{return owner;}
    inline GameEntity(int y, int x, Cell* cell) : yCoord(y), xCoord(x),  cell(cell){health = 1; }
    [[nodiscard]] inline Cell* getCell() const{
        return cell;
    }
    [[nodiscard]] inline int getHealth() const {return health;}
    inline void setCell(Cell* newCell){
        cell = newCell;
    }

    inline void setCoords(int y, int x){
        xCoord = x;
        yCoord = y;
    }
    [[nodiscard]] inline int getX() const{
        return xCoord;
    }
    [[nodiscard]] inline int getY() const{
        return yCoord;
    }
    [[nodiscard]] inline virtual bool isMine() const{return false;}
    [[nodiscard]] inline virtual bool isWall() const{return false;}
    [[nodiscard]] inline virtual bool isTank() const{return false;}
    [[nodiscard]] inline virtual bool isShell() const{return false;}
    [[nodiscard]] inline bool isAlive() const{return health>0;}
    inline void hit() {health--;}
};

class Tank : public GameEntity{
private:
    Direction currDirection;
    int shellCount = 16;
    int stepsSinceLastShot = 100; // Placeholder initial value
    int preparingReverseCounter = -1;// Placeholder initial value
    Action nextStepAction = NoAction;
    TankMode mode = NormalMode;


public:
    Tank(int y, int x, Cell *cell, Direction dir);
    [[nodiscard]] inline enum Direction getDirection() const {return currDirection; }
    [[nodiscard]] inline int getShellCount() const {return shellCount;}
    [[nodiscard]] inline TankMode getMode() const {return mode;}
    [[nodiscard]] inline bool isTank() const override{return true;}
    [[nodiscard]] inline Action peekAction() const{return nextStepAction;}
    [[nodiscard]] bool canFire() const;
    void rotate(int rotationAmount);
    void setMode(TankMode newMode);
    [[nodiscard]] inline bool hasShells() const{return shellCount>0;}
    void fire();
    Action consumeAction();
    void setAction(Action action);
    void tickUpdate();


};

class Shell : public GameEntity{
private:
    const Direction dir;
    bool createdThisTurn = true;
public:
    inline Shell(int y, int x, Cell *cell, Direction dir) : GameEntity(y, x ,cell), dir(dir){}
    [[nodiscard]] inline enum Direction getDirection() const{
        return dir;
    }
    [[nodiscard]] inline bool isNewShell() const{return createdThisTurn;}
    inline void setAsOld(){createdThisTurn = false;}
    [[nodiscard]] inline bool isShell() const override{return true;}
};

class Wall : public GameEntity{

public:
    inline Wall(int y, int x, Cell *cell) : GameEntity(y, x ,cell){health = 2;}
    [[nodiscard]] inline bool isWall() const override{return true;}
};

class Mine : public GameEntity{
public:
    inline Mine(int y, int x, Cell* cell) : GameEntity(y, x, cell){}
    [[nodiscard]] inline bool isMine() const override{return true;}
};