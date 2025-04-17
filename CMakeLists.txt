cmake_minimum_required(VERSION 3.28)
project(tank_game LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

# Option to toggle SFML
option(USE_SFML "Build with SFML graphics support" ON)

if(USE_SFML)
    include(FetchContent)
    FetchContent_Declare(SFML
            GIT_REPOSITORY https://github.com/SFML/SFML.git
            GIT_TAG 3.0.x
            GIT_SHALLOW ON
            EXCLUDE_FROM_ALL
            SYSTEM)
    FetchContent_MakeAvailable(SFML)
endif()



set(SOURCES
        src/tank.cpp
        src/main.cpp
        src/map_loader.cpp
        src/game_map.cpp
        src/game_manager.cpp
        src/cell.cpp
        src/chasing_algorithm.cpp
        src/algorithm_v1.cpp
        src/algorithms.cpp
)


if(USE_SFML)
    list(APPEND SOURCES src/sfml_renderer.cpp)
endif()

add_executable(tank_game ${SOURCES})
target_compile_features(tank_game PRIVATE cxx_std_17)

if(USE_SFML)
    target_link_libraries(tank_game PRIVATE SFML::Graphics)
    target_compile_definitions(tank_game PRIVATE USE_SFML)
endif()
