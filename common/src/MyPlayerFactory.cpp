#include "MyPlayerFactory.h"
#include "PlayerOne.h"
#include "PlayerTwo.h"

unique_ptr<Player> MyPlayerFactory::create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
    if(player_index % 2 == 1){
        return std::make_unique<Algorithm_206038929_314620071::PlayerOne>(player_index, x, y, max_steps, num_shells);

    }
    else{
        return std::make_unique<Algorithm_206038929_314620071::PlayerTwo>(player_index, x, y, max_steps, num_shells);
    }
}