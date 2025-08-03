cmake_minimum_required(VERSION 3.28)
project(GameManager_206038929_314620071 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES
        src/GameManager.cpp
        src/Cell.cpp
        src/EntityManager.cpp
        src/GameMap.cpp
        src/Tank.cpp
)

add_library(GameManager_206038929_314620071 SHARED ${SOURCES})

set_target_properties(GameManager_206038929_314620071 PROPERTIES
        OUTPUT_NAME GameManager_206038929_314620071
        PREFIX ""
)
target_include_directories(GameManager_206038929_314620071 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../UserCommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../common

)


target_compile_options(GameManager_206038929_314620071 PRIVATE
        -Wall -Wextra -Werror -pedantic
)
