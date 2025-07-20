#pragma once
#include "FullBattleInfo.h"

namespace Algorithm_206038929_314620071 {

    class PlayerCommunicationModule {


    protected:
        static constexpr int getInfoCounterWhenAlone = 4;
        int tankCount = 0;
        int playerIndex;
        size_t rows;
        size_t cols;
        size_t maxSteps;
        size_t numShells;
        FullBattleInfo battleInfo;
        std::vector<int> tankIndices = {};
        bool seenAllMyTanks = false;
        std::vector<int> tankIndicesThatRecentlyDied{};

        void checkForDeadTanks();

    public:
        PlayerCommunicationModule(int player_index,
                                  size_t x, size_t y,
                                  size_t max_steps, size_t num_shells) : playerIndex(player_index), rows(y), cols(x),
                                                                         maxSteps(max_steps), numShells(num_shells),
                                                                         battleInfo(rows, cols, numShells,
                                                                                    player_index) {}

        virtual void updateTankInstructions() = 0;

        virtual void processSatelliteView(SatelliteView &satellite_view);

        virtual void processBattleInfoPreSending();

        virtual void processBattleInfoPostSending();

        virtual ~PlayerCommunicationModule() = default;


    };
}