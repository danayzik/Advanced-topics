#pragma once

#include "SatelliteView.h"
#include "BattleInfo.h"
#include <vector>
#include <memory>
#include "ObservedCell.h"
#include <unordered_set>
#include "ObservedEntityUtils.h"
#include "TankRole.h"

namespace Algorithm_206038929_314620071 {
    using namespace EntityUtils;
    using std::unique_ptr;


    class FullBattleInfo : public BattleInfo {
    private:

        size_t rows;
        size_t cols;
        size_t startingShells;
        std::vector<std::vector<ObservedCell>> observedCells = {};
        ObservedTank* myTank = {};
        std::vector<ObservedShell*> shells = {};
        std::vector<ObservedTank*> enemyTanks = {};
        std::vector<ObservedTank*> friendlyTanks = {};
        int playerIndex{};
        int tankIndex{};
        TankRole roleToPlay{};
        int requestInfoCountdown = 0;
        void moveKnownShells();



    public:
        FullBattleInfo();

        FullBattleInfo(size_t rows, size_t cols, size_t startingShells, int playerIndex = 0);

        FullBattleInfo(size_t rows, size_t cols, size_t startingShells, const SatelliteView &satelliteView,
                       int playerIndex);

        FullBattleInfo(const FullBattleInfo& other);

        FullBattleInfo& operator=(const FullBattleInfo& other);


        ~FullBattleInfo() override = default;


        FullBattleInfo &operator=(FullBattleInfo &&other) noexcept = default;

        FullBattleInfo(FullBattleInfo &&) = default;

        void moveMyTankForward();

        void roundTick();

        void updateFromTankInfo(FullBattleInfo &tankInfo);

        void updateFromEarlierInfo(FullBattleInfo &earlierInfo);

        void fireShellFromTank(ObservedTank &tank);

        [[nodiscard]] ObservedTank& getMyTank() { return *myTank; }
        [[nodiscard]] const ObservedTank& getMyTank() const { return *myTank; }

        [[nodiscard]] std::vector<ObservedShell*>& getShells() { return shells; }
        [[nodiscard]] const std::vector<ObservedShell*>& getShells() const { return shells; }

        [[nodiscard]] std::vector<ObservedTank*>& getEnemyTanks() { return enemyTanks; }
        [[nodiscard]] const std::vector<ObservedTank*>& getEnemyTanks() const { return enemyTanks; }

        [[nodiscard]] std::vector<ObservedTank*>& getFriendlyTanks() { return friendlyTanks; }
        [[nodiscard]] const std::vector<ObservedTank*>& getFriendlyTanks() const { return friendlyTanks; }


        [[nodiscard]] inline size_t getRows() const { return rows; }

        [[nodiscard]] inline size_t getCols() const { return cols; }

        [[nodiscard]] inline size_t getStartingShells() const { return startingShells; }

        [[nodiscard]] inline const ObservedCell &getCell(Coordinates coords) const { return observedCells[coords.yAsSizeT()][coords.xAsSizeT()]; }

        [[nodiscard]] inline ObservedCell &getCell(Coordinates coords) { return observedCells[coords.yAsSizeT()][coords.xAsSizeT()]; }

        [[nodiscard]] inline const ObservedCell &getCell(size_t y, size_t x) const { return observedCells[y][x]; }

        [[nodiscard]] inline ObservedCell &getCell(size_t y, size_t x) { return observedCells[y][x]; }

        [[nodiscard]] inline const std::vector<std::vector<ObservedCell>> &getGrid() const { return observedCells; }

        [[nodiscard]] inline std::vector<std::vector<ObservedCell>> &getGrid() { return observedCells; }

        [[nodiscard]] inline Coordinates getMyTankCoords() const { return myTank->getCoords(); }

        [[nodiscard]] inline int getTankIndex() const { return tankIndex; }

        [[nodiscard]] std::unordered_set<Coordinates, CoordinatesHash> getEnemyTanksCoordinates() const;

        void setTankIndex(int index);

        inline void setRole(TankRole role) { roleToPlay = role; }

        [[nodiscard]] inline TankRole getRole() const { return roleToPlay; }

        [[nodiscard]] inline bool shouldRequestInfo() const { return requestInfoCountdown <= 0; }

        inline void setRequestInfoCounter(int turns) { requestInfoCountdown = turns; }


        void assumeEnemyShellsDirections(int distanceFromTank);

        void cloneCell(const FullBattleInfo& other, size_t y, size_t x);
    };
}