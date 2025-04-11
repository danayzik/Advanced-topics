cmake_minimum_required(VERSION 3.10)
project(tank_game)

set(CMAKE_CXX_STANDARD 17)

# === Option to enable SFML (visuals) ===
option(USE_VISUALS "Enable SFML graphics" ON)

# === Include SFML if visuals are enabled ===
if(USE_VISUALS)
    set(SFML_DIR "D:/libraries/SFML-3.0.0/lib/cmake/SFML")
    # Ensure we're requesting components correctly as per SFML 3.0.0
    find_package(SFML 3 REQUIRED COMPONENTS System Window Graphics)  # Use the required components
    add_definitions(-DUSE_VISUALS)
endif()

# === Sources ===
set(SOURCES
        tank.cpp
        main.cpp
        map_loader.cpp
        game_map.cpp
        game_manager.cpp
        cell.cpp
        player_v1.cpp
        chasing_player.cpp
)

# === Add SFML renderer if visuals are enabled ===
if(USE_VISUALS)
    list(APPEND SOURCES sfml_renderer.cpp)
endif()

add_executable(tank_game ${SOURCES})

# === Link SFML if visuals are enabled ===
if(USE_VISUALS)
    target_include_directories(tank_game PRIVATE D:/libraries/SFML-3.0.0/include)
    target_link_libraries(tank_game PRIVATE SFML::Graphics SFML::Window SFML::System)  # Use targets from SFML 3
endif()
