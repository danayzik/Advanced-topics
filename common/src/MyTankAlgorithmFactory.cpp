#include "MyTankAlgorithmFactory.h"
#include "AdaptiveTankAlgorithm.h"



unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {

    return std::make_unique<Algorithm_206038929_314620071::AdaptiveTankAlgorithm>(player_index, tank_index);

}

