#include "MyTankAlgorithmFactory.h"
#include "AdaptiveTankAlgorithm.h"



unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {

    return std::make_unique<AdaptiveTankAlgorithm>(player_index, tank_index);



}

