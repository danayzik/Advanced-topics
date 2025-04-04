#include <unordered_set>
#include <iostream>
#include "game_entity.h"
#include "cell.h"
#include "direction.h"
using std::vector;
using std::unordered_set;
using std::string;
class GameMap {
private:

    vector<vector<Cell>> grid;
    unordered_set<GameEntity*> tanks;
    unordered_set<GameEntity*> shells;
    int rows;
    int cols;

public:
    GameMap(int rows, int cols) : rows(rows), cols(cols), grid(rows, vector<Cell>(cols)) {}

    void readMapFromFile(string filePath){
        return; //later
    }

    void moveEntity(GameEntity* entity, Direction dir){
        Cell* currCell = entity->getCell();
    }



};
