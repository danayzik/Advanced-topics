cmake_minimum_required(VERSION 3.10)
project(tank_game)

set(CMAKE_CXX_STANDARD 17)

add_executable(tank_game
        tank.cpp
        main.cpp
        map_loader.cpp
        game_map.cpp
        game_manager.cpp
)
