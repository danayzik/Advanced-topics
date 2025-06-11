#pragma once
#include <cstddef>
#include <utility>
#include "Direction.h"
#include <optional>
#include <memory>
#include "TankMode.h"
enum class EntityType{
    Tank, Shell, Wall
};


class ObservedEntity{

protected:

    Coordinates coords;


public:
    inline virtual ~ObservedEntity() = default;
    inline ObservedEntity(size_t y, size_t x) :  coords{y, x}{}
    inline explicit ObservedEntity(Coordinates coords) :  coords(coords){}
    [[nodiscard]] virtual std::unique_ptr<ObservedEntity> clone() const = 0;
    inline void setCoords(size_t y, size_t x){
        coords = {y, x};
    }
    inline void setCoords(Coordinates newCoords){coords = newCoords;}
    [[nodiscard]] inline Coordinates getCoords() const{return coords;}
    [[nodiscard]] inline int getX() const{
        return coords.x;
    }
    [[nodiscard]] inline int getY() const{
        return coords.y;
    }
    [[nodiscard]] inline size_t getXAsSizeT() const{
        return coords.xAsSizeT();
    }
    [[nodiscard]] inline size_t getYAsSizeT() const{
        return coords.yAsSizeT();
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
    int tankIndex{};


public:
    ObservedTank(size_t y, size_t x, int playerIndex, size_t shellCount, std::optional<Direction> dir = std::nullopt): ObservedEntity(y, x), playerIndex(playerIndex), shellCount(shellCount), assumedDirection(dir){}
    ObservedTank(Coordinates coords, int playerIndex, size_t shellCount, std::optional<Direction> dir = std::nullopt): ObservedEntity(coords), playerIndex(playerIndex), shellCount(shellCount), assumedDirection(dir){}
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
    [[nodiscard]] inline int getIndex() const {return tankIndex;};
    inline void setIndex(int newIndex){tankIndex = newIndex;}
};


class ObservedShell : public ObservedEntity{
private:

    std::optional<Direction> assumedDirection;
    EntityType type = EntityType::Shell;
    bool firedLastTurn;
public:
    inline explicit ObservedShell(Coordinates coords,  std::optional<Direction> assumedDirection = std::nullopt, bool firedLastTurn = false): ObservedEntity(coords), assumedDirection(assumedDirection), firedLastTurn(firedLastTurn){}
    inline ObservedShell(size_t y, size_t x,  std::optional<Direction> assumedDirection = std::nullopt, bool firedLastTurn = false): ObservedEntity(y,x), assumedDirection(assumedDirection), firedLastTurn(firedLastTurn){}

    [[nodiscard]] inline std::optional<Direction> getDirection() const {return assumedDirection; }
    [[nodiscard]] inline bool isShell() const override{return true;}
    [[nodiscard]] inline std::unique_ptr<ObservedEntity> clone() const override {
        return std::make_unique<ObservedShell>(*this);
    }
    inline void setDirection(Direction dir){assumedDirection = dir;};
    [[nodiscard]] inline bool directionKnown()const {return assumedDirection.has_value();}
    [[nodiscard]] inline EntityType getType() const override{return type;};
    [[nodiscard]] inline bool getFiredLastTurn() const{return firedLastTurn;};
    inline void setAsOld() {firedLastTurn = true;}
};

class ObservedWall : public ObservedEntity{
    EntityType type = EntityType::Wall;
public:

    inline ObservedWall(size_t y, size_t x) : ObservedEntity(y, x){}
    inline explicit  ObservedWall(Coordinates coords) : ObservedEntity(coords){}
    [[nodiscard]] inline bool isWall() const override{return true;}
    [[nodiscard]] inline std::unique_ptr<ObservedEntity> clone() const override {
        return std::make_unique<ObservedWall>(*this);
    }
    [[nodiscard]] inline EntityType getType() const override{return type;};
};



