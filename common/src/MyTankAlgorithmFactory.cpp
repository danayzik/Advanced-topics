#include "MyTankAlgorithmFactory.h"
#include "FirstTankAlgorithm.h"



unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {

    return std::make_unique<FirstTankAlgorithm>(player_index, tank_index);



}

