cmake_minimum_required(VERSION 3.10)
project(SDL_cpp)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set the output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/build)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Find SDL2
find_package(SDL2 REQUIRED)

# Globbing source files
file(GLOB_RECURSE SOURCES "src/*.cc" "src/*.cpp")

# Globbing header files
file(GLOB_RECURSE HEADERS "include/*.hh" "include/*.hpp")

# Add executable
add_executable(${PROJECT_NAME} ${SOURCES})

# Specify compiler options for GCC
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()

# Set optimization level
add_compile_options(-O2)

# Add include directories recursively
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Add include directories for SDL2
include_directories(${SDL2_INCLUDE_DIRS})

# Link SDL2 libraries
target_link_libraries(${PROJECT_NAME} PRIVATE ${SDL2_LIBRARIES})

# Copy assets to build directory
file(COPY assets DESTINATION ${CMAKE_BINARY_DIR})

