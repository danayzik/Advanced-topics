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
        src/tank.cpp
        src/main.cpp
        src/map_loader.cpp
        src/game_map.cpp
        src/game_manager.cpp
        src/cell.cpp
        src/chasing_algorithm.cpp
        src/algorithm_v1.cpp
        src/algorithms.cpp
        src/entity_manager.cpp
)


if(USE_SFML)
    list(APPEND SOURCES src/sfml_renderer.cpp)
endif()

add_executable(tanks_game ${SOURCES})
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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
