cmake_minimum_required(VERSION 3.10)
project(TankGame)

set(CMAKE_CXX_STANDARD 17)

add_executable(TankGame
        tank.cpp
        game_map.cpp
)
