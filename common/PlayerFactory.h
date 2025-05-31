#pragma once
#include "Player.h"
#include <memory>
using std::unique_ptr;

class PlayerFactory {
public:
    virtual ~PlayerFactory() = default;
    [[nodiscard]] virtual unique_ptr<Player> create(int player_index, size_t x, size_t y,
                                      size_t max_steps, size_t num_shells ) const = 0;
};
