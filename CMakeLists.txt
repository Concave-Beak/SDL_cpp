cmake_minimum_required(VERSION 3.22)

project(game)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_COMPILER g++)                      
add_definitions(-g -O2 -Wall -Wextra) 

# Dependencies
find_package(SDL2 REQUIRED CONFIG REQUIRED COMPONENTS SDL2)
##############

file (GLOB SOURCES
    "./Source/*/*.cc"
    )

file (GLOB HEADERS
    "./Source/*/*.hh"
    )


add_executable(${PROJECT_NAME} ${SOURCES})

target_link_libraries(${PROJECT_NAME} PRIVATE ${HEADERS} SDL2::SDL2)
