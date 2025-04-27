#pragma once
#include <memory>
#include <unordered_set>
#include <vector>
#include <string>
#include <fstream>
#include "cell.h"
#include "direction.h"
#include "entities.h"
#include "renderer.h"
using std::unordered_set;
using std::vector;
using std::string;
class MapLoader;
enum GameResult{
    PlayerOneWin,
    PlayerTwoWin,
    Draw,
    NotOver
};

class GameMap {
private:
    vector<vector<Cell>> grid;
    unordered_set<Tank*> playerOneTanks;
    unordered_set<Tank*> playerTwoTanks;
    unordered_set<Shell*> shells;
    std::unique_ptr<Renderer> renderer;
    int rows;
    int cols;
    std::pair<int, int> getNewPosition(const GameEntity* entity, Direction dir) const;
    void resolveCollisions(const unordered_set<Cell*>& dirtyCells);



public:
    explicit GameMap(const string& filePath);
    ~GameMap();
    bool tankCanMoveInDirection(const Tank* tank, Direction dir) const;
    void moveEntity(GameEntity* entity, Direction dir);
    void moveShells();
    void createShell(Tank* tank);
    void checkCollisions();
    bool tanksAboutToCollide(Tank* tank1, Tank* tank2);
    void shellsAboutToCollide();
    [[nodiscard]] GameResult getGameResult() const;
    [[nodiscard]] const Tank* getEnemyTank(int playerNumber)const;
    [[nodiscard]] inline int getRows() const{return rows;}
    [[nodiscard]] inline int getCols() const{return cols;}
    void updateVisuals();
    [[nodiscard]] inline const vector<vector<Cell>>& getGrid() const{return grid;}
    [[nodiscard]] inline const unordered_set<Shell*>& getShells() const{return shells;}
    friend MapLoader;
    inline std::pair<Tank*, Tank*> getPlayerTanks(){
        return {*playerOneTanks.begin(),*playerTwoTanks.begin()};}

};
