# Requires homebrew bcc with libomp on macOS.
if (APPLE)
    set(CMAKE_CXX_COMPILER /usr/local/bin/c++-10)
endif()

project(RouteKit CXX)
cmake_minimum_required(VERSION 3.18)

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)

file(GLOB_RECURSE SOURCES src/*.cpp)

include_directories(include)

find_package(OpenMP REQUIRED)

include_directories(include ${OpenMP_CXX_INCLUDES})
add_compile_options(${OpenMP_CXX_FLAGS})
add_link_options(${OpenMP_CXX_LIB_NAMES})

add_library(RouteKit ${SOURCES})
target_compile_features(RouteKit PRIVATE cxx_std_17)

