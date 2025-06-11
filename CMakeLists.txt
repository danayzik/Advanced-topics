cmake_minimum_required(VERSION 3.28)
project(tanks_game LANGUAGES CXX)

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
        src/main.cpp
        game_side/src/Tank.cpp
        game_side/src/MapLoader.cpp
        game_side/src/GameMap.cpp
        game_side/src/GameManager.cpp
        game_side/src/Cell.cpp
        game_side/src/EntityManager.cpp
        players_side/src/FullBattleInfo.cpp
        players_side/src/ObservedCell.cpp
        players_side/src/AlgorithmUtilities.cpp
        players_side/src/FirstTankAlgorithm.cpp
        players_side/src/ObservedTank.cpp
        players_side/src/PlayerOne.cpp
        players_side/src/PlayerTwo.cpp
        common/src/MyPlayerFactory.cpp
        common/src/MyTankAlgorithmFactory.cpp
        players_side/src/ChasingTank.cpp
        players_side/src/SnipingTank.cpp
        players_side/include/TankRole.h
)


if(USE_SFML)
    list(APPEND SOURCES game_side/src/SFMLRenderer.cpp)
endif()

add_executable(tanks_game ${SOURCES})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(tanks_game PRIVATE
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/players_side/include
        ${CMAKE_SOURCE_DIR}/game_side/include
)

target_compile_options(tanks_game PRIVATE
        -Wall
        -Wextra
        -Werror
        -pedantic
)

if(USE_SFML)
    target_link_libraries(tanks_game PRIVATE SFML::Graphics)
    target_compile_definitions(tanks_game PRIVATE USE_SFML)
endif()
