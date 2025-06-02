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
#include "entity_manager.h"
#include "../common/SatelliteView.h"
#include "../common/ConcreteSatelliteView.h"
using std::unordered_set;
using std::vector;
using std::string;
class GameManager;
class MapLoader;


class GameMap {
private:
    vector<vector<Cell>> grid = {};
    unordered_set<size_t> shellsIds = {};
    unordered_set<size_t> tankIds = {};
    std::unique_ptr<Renderer> renderer = {};
    EntityManager entityManager = {};
    size_t rows = 0;
    size_t cols = 0;
    std::pair<size_t, size_t> getNewPosition(const GameEntity& entity, Direction dir) const;
    void resolveCollisions(const unordered_set<Cell*>& dirtyCells);




public:
    GameMap() = default;
    void readBoard(const string& mapFilePath, GameManager& gameManager);
    bool tankCanMoveInDirection(const Tank& tank, Direction dir) const;
    void moveShells();
    void fireShell(const Tank& tank);
    void checkCollisions();
    void tanksAboutToCollide();
    void shellsAboutToCollide();
    void moveEntity(GameEntity& entity, Direction dir);
    inline Tank& getTank(size_t tankEntityIndex) {return dynamic_cast<Tank&>(entityManager.getEntity(tankEntityIndex));}
    std::unique_ptr<SatelliteView> getSatelliteView(const Tank& tank);
    [[nodiscard]] inline size_t getRows() const{return rows;}
    [[nodiscard]] inline size_t getCols() const{return cols;}
    void updateVisuals();
    friend MapLoader;
    inline bool isTankAlive(size_t entityIndex) const {return entityManager.isTankAlive(entityIndex);}

};
