cmake_minimum_required(VERSION 3.28)
project(simulator_206038929_314620071 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})


file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


add_executable(simulator_206038929_314620071 ${SOURCES} )

target_link_options(simulator_206038929_314620071 PRIVATE -rdynamic)
target_include_directories(simulator_206038929_314620071 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/../UserCommon
        ${CMAKE_CURRENT_SOURCE_DIR}/../common
)


target_compile_options(simulator_206038929_314620071 PRIVATE
        -Wall -Wextra -Werror -pedantic
)
