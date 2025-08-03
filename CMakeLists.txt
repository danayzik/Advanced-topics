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
        GameManager/src/Tank.cpp
        GameManager/src/MapLoader.cpp
        GameManager/src/GameMap.cpp
        GameManager/src/GameManager.cpp
        GameManager/src/Cell.cpp
        GameManager/src/EntityManager.cpp
        Algorithm/src/FullBattleInfo.cpp
        Algorithm/src/ObservedCell.cpp
        Algorithm/src/AlgorithmUtilities.cpp
        Algorithm/src/AdaptiveTankAlgorithm.cpp
        Algorithm/src/ObservedTank.cpp
        Algorithm/src/PlayerOne.cpp
        Algorithm/src/PlayerTwo.cpp
        Algorithm/src/PlayerCommunicationModule.cpp
        Algorithm/src/ChasingTank.cpp
        Algorithm/src/SnipingTank.cpp
        Algorithm/src/PlayerCommunicationModule.cpp
        Simulator/src/CommandLineParser.cpp
        Simulator/src/MapLoader.cpp
        Simulator/src/main.cpp
        Simulator/src/CompetitionSimulator.cpp
        Simulator/src/ComparativeSimulator.cpp
        Simulator/src/AlgorithmRegistrar.cpp
        Simulator/src/GameManagerRegistrar.cpp

)


if(USE_SFML)
    list(APPEND SOURCES GameManager/src/SFMLRenderer.cpp)
endif()

add_executable(tanks_game ${SOURCES})


set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

target_include_directories(tanks_game PRIVATE
        ${CMAKE_SOURCE_DIR}/common
        ${CMAKE_SOURCE_DIR}/Algorithm/include
        ${CMAKE_SOURCE_DIR}/GameManager/include
        ${CMAKE_SOURCE_DIR}/UserCommon
        ${CMAKE_SOURCE_DIR}/Simulator/include
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
