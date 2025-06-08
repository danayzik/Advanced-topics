#include "MyPlayerFactory.h"
#include "PlayerOne.h"

unique_ptr<Player>
MyPlayerFactory::create(int player_index, size_t x, size_t y, size_t max_steps, size_t num_shells) const {
    return std::make_unique<PlayerOne>(player_index, x, y, max_steps, num_shells);
}