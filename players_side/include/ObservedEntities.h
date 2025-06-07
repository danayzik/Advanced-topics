#pragma once
#include <cstddef>
#include <utility>
#include "Direction.h"
#include <optional>
#include <memory>
enum TankMode {
    NormalMode,
    PreparingReverse,
    JustEnteredReverse,
    ReverseMode
};

enum class EntityType{
    Tank, Shell, Wall
};

class ObservedEntity{

protected:

    size_t yCoord;
    size_t xCoord;


public:
    inline virtual ~ObservedEntity() = default;
    inline ObservedEntity(size_t y, size_t x) :  yCoord(y), xCoord(x){}
    [[nodiscard]] virtual std::unique_ptr<ObservedEntity> clone() const = 0;
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

    [[nodiscard]] inline virtual bool isWall() const{return false;}
    [[nodiscard]] inline virtual bool isTank() const{return false;}
    [[nodiscard]] inline virtual bool isShell() const{return false;}
    [[nodiscard]] inline virtual EntityType getType() const = 0;

};

class ObservedTank : public ObservedEntity{
private:
    static int constexpr firingCooldown = 4;


    int stepsSinceLastShot = 100; // Placeholder initial value
    int preparingReverseCounter = -1;
    int playerIndex;
    size_t shellCount;
    TankMode mode = TankMode::NormalMode;
    EntityType type = EntityType::Tank;
    std::optional<Direction> assumedDirection;

public:
    ObservedTank(size_t y, size_t x, int playerIndex, size_t shellCount, std::optional<Direction> dir = std::nullopt): ObservedEntity(y, x), playerIndex(playerIndex), shellCount(shellCount), assumedDirection(dir){}
    [[nodiscard]] inline std::optional<Direction> getDirection() const {return assumedDirection;}
    [[nodiscard]] inline size_t getShellCount() const {return shellCount;}
    [[nodiscard]] inline bool isTank() const override{return true;}
    [[nodiscard]] bool canFire() const;
    [[nodiscard]] inline int getPlayerIndex() const {return playerIndex;}
    [[nodiscard]] inline bool hasShells() const{return shellCount>0;}
    [[nodiscard]] inline bool ownedByPlayer(int pIndex) const{return playerIndex == pIndex;}
    void tickUpdate();
    void fire();
    [[nodiscard]] inline bool directionKnown()const {return assumedDirection.has_value();}
    void rotate(int rotationAmount);
    [[nodiscard]] inline std::unique_ptr<ObservedEntity> clone() const override {
        auto copy = std::make_unique<ObservedTank>(*this);
        return copy;
    }
    inline void setDirection(Direction dir){assumedDirection = dir;};
    [[nodiscard]] inline EntityType getType() const override{return type;};
};


class ObservedShell : public ObservedEntity{
private:

    std::optional<Direction> assumedDirection;
    EntityType type = EntityType::Shell;

public:

    inline ObservedShell(size_t y, size_t x,  std::optional<Direction> assumedDirection = std::nullopt): ObservedEntity(y,x), assumedDirection(assumedDirection){}

    [[nodiscard]] inline std::optional<Direction> getDirection() const {return assumedDirection; }
    [[nodiscard]] inline bool isShell() const override{return true;}
    [[nodiscard]] inline std::unique_ptr<ObservedEntity> clone() const override {
        return std::make_unique<ObservedShell>(*this);
    }
    inline void setDirection(Direction dir){assumedDirection = dir;};
    [[nodiscard]] inline bool directionKnown()const {return assumedDirection.has_value();}
    [[nodiscard]] inline EntityType getType() const override{return type;};
};

class ObservedWall : public ObservedEntity{
    EntityType type = EntityType::Wall;
public:

    inline ObservedWall(size_t y, size_t x) : ObservedEntity(y, x){;}
    [[nodiscard]] inline bool isWall() const override{return true;}
    [[nodiscard]] inline std::unique_ptr<ObservedEntity> clone() const override {
        return std::make_unique<ObservedWall>(*this);
    }
    [[nodiscard]] inline EntityType getType() const override{return type;};
};



