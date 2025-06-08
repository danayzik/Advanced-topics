#pragma once

#include "SatelliteView.h"
#include "BattleInfo.h"
#include <vector>
#include <memory>
#include "ObservedCell.h"
#include <unordered_set>
using std::unique_ptr;
class FullBattleInfo : public BattleInfo {
private:

    size_t rows;
    size_t cols;
    size_t startingShells;
    std::vector<std::vector<ObservedCell>> observedCells;
    Coordinates myTankCoords = {};
    std::unordered_set<Coordinates, CoordinatesHash> shellCoordinates;
    std::unordered_set<Coordinates, CoordinatesHash> friendlyTanksCoordinates;
    std::unordered_set<Coordinates, CoordinatesHash> enemyTanksCoordinates;



public:
    FullBattleInfo();
    FullBattleInfo(size_t rows, size_t cols, size_t startingShells);
    FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView& satelliteView, int playerIndex);


    ~FullBattleInfo() override = default;
    FullBattleInfo(const FullBattleInfo& other) = default;
    FullBattleInfo& operator=(const FullBattleInfo& other) = default;
    FullBattleInfo& operator=(FullBattleInfo&& other) noexcept = default;
    FullBattleInfo(FullBattleInfo&&) = default;


    void updateFromTankInfo(FullBattleInfo& tankInfo);
    void updateFromEarlierInfo(FullBattleInfo& earlierInfo);
    void moveKnownShells();
    void moveMyTankForward();
    void fireShellFromTank(ObservedTank& tank);
    [[nodiscard]] inline size_t getRows() const{return rows;}
    [[nodiscard]] inline size_t getCols() const{return cols;}
    [[nodiscard]] inline size_t getStartingShells() const{return startingShells;}

    [[nodiscard]] inline const ObservedCell& getCell(Coordinates coords) const{return observedCells[coords.yAsSizeT()][coords.xAsSizeT()];}
    [[nodiscard]] inline ObservedCell& getCell(Coordinates coords){return observedCells[coords.yAsSizeT()][coords.xAsSizeT()];}
    [[nodiscard]] inline const ObservedCell& getCell(size_t y, size_t x) const{return observedCells[y][x];}
    [[nodiscard]] inline ObservedCell& getCell(size_t y, size_t x){return observedCells[y][x];}

    [[nodiscard]] inline const std::vector<std::vector<ObservedCell>>& getGrid() const{return observedCells;}
    [[nodiscard]] inline std::vector<std::vector<ObservedCell>>& getGrid() {return observedCells;}
    [[nodiscard]] inline const std::unordered_set<Coordinates, CoordinatesHash>& getShellsCoordinates() const{return shellCoordinates;}
    [[nodiscard]] inline std::unordered_set<Coordinates, CoordinatesHash>& getShellsCoordinates() {return shellCoordinates;}
    [[nodiscard]] inline const ObservedTank& getMyTank() const {return dynamic_cast<const ObservedTank&>(*(getCell(myTankCoords).entity));}
    [[nodiscard]] inline ObservedTank& getMyTank()  {return dynamic_cast<ObservedTank&>(*(getCell(myTankCoords).entity));}
    [[nodiscard]] inline Coordinates getMyTankCoords() const{return myTankCoords;}
    [[nodiscard]] inline const std::unordered_set<Coordinates, CoordinatesHash>& getEnemyTanksCoordinates() const{return enemyTanksCoordinates;}
    [[nodiscard]] inline std::unordered_set<Coordinates, CoordinatesHash>& getEnemyTanksCoordinates() {return enemyTanksCoordinates;}
    [[nodiscard]] inline const std::unordered_set<Coordinates, CoordinatesHash>& getFriendlyTanksCoordinates() const {return friendlyTanksCoordinates;}
    [[nodiscard]] inline std::unordered_set<Coordinates, CoordinatesHash>& getFriendlyTanksCoordinates()  {return friendlyTanksCoordinates;}
};