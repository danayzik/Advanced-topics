#pragma once
#include "TankAlgorithmFactory.h"

using std::unique_ptr;

class MyTankAlgorithmFactory : public TankAlgorithmFactory {
public:
    [[nodiscard]] unique_ptr<TankAlgorithm> create(
            int player_index, int tank_index) const override;


};
