#include "MyTankAlgorithmFactory.h"
#include "FirstTankAlgorithm.h"
#include "SecondTankAlgorithm.h"


unique_ptr<TankAlgorithm> MyTankAlgorithmFactory::create(int player_index, int tank_index) const {
    if(player_index % 2 == 1){
        return std::make_unique<FirstTankAlgorithm>(player_index, tank_index);

    }
    else{
        return std::make_unique<SecondTankAlgorithm>(player_index, tank_index);
    }
}

