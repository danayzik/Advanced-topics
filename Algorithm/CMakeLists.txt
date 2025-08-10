cmake_minimum_required(VERSION 3.28)
project(Algorithm_206038929_314620071 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
        src/AlgorithmUtilities.cpp
        src/ChasingTank.cpp
        src/FullBattleInfo.cpp
        src/ObservedCell.cpp
        src/ObservedTank.cpp
        src/Player_206038929_314620071.cpp
        src/SnipingTank.cpp
        src/TankAlgorithm_206038929_314620071.cpp
)


add_library(Algorithm_206038929_314620071 SHARED ${SOURCES})

set_target_properties(Algorithm_206038929_314620071 PROPERTIES
        OUTPUT_NAME Algorithm_206038929_314620071
        PREFIX ""
)
target_include_directories(Algorithm_206038929_314620071 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../UserCommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../common

)


target_compile_options(Algorithm_206038929_314620071 PRIVATE
        -Wall -Wextra -Werror -pedantic
)
