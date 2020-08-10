# Requires homebrew bcc with libomp on macOS.
if (APPLE)
    set(CMAKE_CXX_COMPILER /usr/local/bin/c++-10)
endif()

project(RouteKit CXX)
cmake_minimum_required(VERSION 3.18)

enable_testing()

set(CXX_STANDARD 17)
set(CXX_STANDARD_REQUIRED ON)
set(CXX_EXTENSIONS OFF)


# May CMake forgive the use of GLOB to locate source files.
file(GLOB_RECURSE LIB_SOURCES src/lib/*.cpp)
file(GLOB_RECURSE TEST_SOURCES src/tests/*.cpp)
file(GLOB_RECURSE PROG_SOURCES src/programs/*.cpp)

include_directories(include src/lib)

find_package(OpenMP REQUIRED)

include_directories(include ${OpenMP_CXX_INCLUDES})
add_compile_options(${OpenMP_CXX_FLAGS})


add_library(RouteKit ${LIB_SOURCES})
target_compile_features(RouteKit PRIVATE cxx_std_17)
target_link_libraries(RouteKit ${OpenMP_CXX_LIB_NAMES})

set(EXE_LIBS RouteKit pthread m z)

add_custom_target(all_programs)
foreach(PROG ${PROG_SOURCES})
    get_filename_component(PROG_NAME ${PROG} NAME_WE)
    add_executable(${PROG_NAME} ${PROG})
    target_link_libraries(${PROG_NAME} ${EXE_LIBS})
    add_dependencies(all_programs ${PROG_NAME})
endforeach()

foreach(TEST_PROG ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_PROG} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_PROG})
    target_link_libraries(${TEST_NAME} ${EXE_LIBS})
    add_test(${TEST_NAME} COMMAND ${TEST_NAME})
endforeach()

