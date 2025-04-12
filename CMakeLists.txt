cmake_minimum_required(VERSION 3.28)
project(tank_game LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(FetchContent)
FetchContent_Declare(SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG 3.0.x
        GIT_SHALLOW ON
        EXCLUDE_FROM_ALL
        SYSTEM)
FetchContent_MakeAvailable(SFML)

add_executable(tank_game tank.cpp
        main.cpp
        map_loader.cpp
        game_map.cpp
        game_manager.cpp
        cell.cpp
        player_v1.cpp
        chasing_player.cpp)
target_compile_features(tank_game PRIVATE cxx_std_17)
target_link_libraries(tank_game PRIVATE SFML::Graphics)




