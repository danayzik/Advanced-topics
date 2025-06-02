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


class GameEntity{

protected:
    size_t entityId;
    size_t yCoord;
    size_t xCoord;
    int health;


public:
    inline virtual ~GameEntity()= default;
    [[nodiscard]] virtual inline char getSymbol() const = 0;
    [[nodiscard]] inline size_t getEntityId() const{return entityId;}
    inline GameEntity(size_t entityId, size_t y, size_t x) : entityId(entityId), yCoord(y), xCoord(x){health = 1;}
    [[nodiscard]] inline int getHealth() const {return health;}

    inline void setCoords(size_t y, size_t x){
        xCoord = x;
        yCoord = y;
    }
    [[nodiscard]] inline std::pair<size_t, size_t> getCoords() const{return {yCoord, xCoord};}
    [[nodiscard]] inline size_t getX() const{
        return xCoord;
    }
    [[nodiscard]] inline size_t getY() const{
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
    ActionRequest nextStepAction = ActionRequest::DoNothing;
    TankMode mode = NormalMode;
    int playerIndex;
    int tankIndex;

public:
    [[nodiscard]] inline char getSymbol() const override {return static_cast<char>('0' + playerIndex);}
    Tank(size_t entityId, size_t y, size_t x, Direction dir, int playerIndex, int tankIndex);
    [[nodiscard]] inline enum Direction getDirection() const {return currDirection; }
    [[nodiscard]] inline int getShellCount() const {return shellCount;}
    [[nodiscard]] inline TankMode getMode() const {return mode;}
    [[nodiscard]] inline bool isTank() const override{return true;}
    [[nodiscard]] inline ActionRequest peekAction() const{return nextStepAction;}
    [[nodiscard]] bool canFire() const;
    [[nodiscard]] inline int getTankIndex() const {return tankIndex;}
    [[nodiscard]] inline int getPlayerIndex() const {return playerIndex;}
    void rotate(int rotationAmount);
    void setMode(TankMode newMode);
    [[nodiscard]] inline bool hasShells() const{return shellCount>0;}
    void fire();
    ActionRequest consumeAction();
    void setAction(ActionRequest action);
    void tickUpdate();


};

class Shell : public GameEntity{
private:
    const Direction dir;
    bool createdThisTurn = true;
public:
    [[nodiscard]] inline char getSymbol() const override {return '*';}
    inline Shell(size_t entityId, size_t y, size_t x, Direction dir) : GameEntity(entityId, y, x), dir(dir){}
    [[nodiscard]] inline enum Direction getDirection() const{
        return dir;
    }
    [[nodiscard]] inline bool isNewShell() const{return createdThisTurn;}
    inline void setAsOld(){createdThisTurn = false;}
    [[nodiscard]] inline bool isShell() const override{return true;}
};

class Wall : public GameEntity{

public:
    [[nodiscard]] inline char getSymbol() const override {return '#';}
    inline Wall(size_t entityId, size_t y, size_t x) : GameEntity(entityId, y, x){health = 2;}
    [[nodiscard]] inline bool isWall() const override{return true;}
};

class Mine : public GameEntity{
public:
    [[nodiscard]] inline char getSymbol() const override {return '@';}
    inline Mine(size_t entityId, size_t y, size_t x) : GameEntity(entityId, y, x){}
    [[nodiscard]] inline bool isMine() const override{return true;}
};