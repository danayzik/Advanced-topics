#pragma once
#include "GameResult.h"
#include <string>
#include "Map.h"

struct GameResultKey {
    int winner;
    GameResult::Reason reason;
    size_t rounds;
    std::vector<size_t> remainingTanks;
    bool crashed;
    std::string finalView{};
    bool operator==(const GameResultKey& other) const {
        if (crashed && other.crashed)
            return true;
        return winner == other.winner &&
               reason == other.reason &&
               remainingTanks == other.remainingTanks &&
               rounds == other.rounds &&
               finalView == other.finalView;
    }

    GameResultKey() = default;

    GameResultKey(const GameResult& gameResult, size_t mapRows, size_t mapCols, bool crashed = false)
            : winner(gameResult.winner),
              reason(gameResult.reason),
              rounds(gameResult.rounds),
              remainingTanks(gameResult.remaining_tanks),
              crashed(crashed) {
        if(!crashed){
            finalView = Map::getStringFromView(*gameResult.gameState, mapRows, mapCols);
        }

    }
};

namespace std {
    template <>
    struct hash<GameResultKey> {
        size_t operator()(const GameResultKey& k) const {
            if (k.crashed)
                return hash<bool>()(k.crashed);
            size_t h1 = hash<int>()(k.winner);
            size_t h2 = hash<int>()(static_cast<int>(k.reason));
            size_t h3 = hash<size_t>()(k.rounds);
            size_t h4 = hash<string>()(k.finalView);

            size_t h5 = 0;
            for (auto tank : k.remainingTanks)
                h5 ^= hash<size_t>()(tank) + 0x9e3779b9 + (h5 << 6) + (h5 >> 2);

            return h1 ^ h2 ^ h3 ^ h4 ^ h5;
        }
    };
}