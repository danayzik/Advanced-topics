#pragma once
#include <string>
#include <functional>
#include <memory>
#include "Player.h"
#include "GameResult.h"

using std::string;

class AbstractGameManager {
public:
	virtual ~AbstractGameManager() = default;
    virtual GameResult run(
    size_t map_width, size_t map_height,
    const SatelliteView& map,
    string map_name,
    size_t max_steps, size_t num_shells,
    Player& player1, string name1, Player& player2, string name2,
    TankAlgorithmFactory player1_tank_algo_factory,
    TankAlgorithmFactory player2_tank_algo_factory) = 0;
};

using GameManagerFactory = std::function<std::unique_ptr<AbstractGameManager>(bool verbose)>;