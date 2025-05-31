#pragma once
#include "TankAlgorithm.h"
#include <memory>
using std::unique_ptr;

class TankAlgorithmFactory {
public:
    virtual ~TankAlgorithmFactory() = default;
    [[nodiscard]] virtual unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index) const = 0;
};
