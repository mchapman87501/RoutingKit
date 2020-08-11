# Requires homebrew bcc with libomp on macOS.
if (APPLE)
    set(CMAKE_CXX_COMPILER /usr/local/bin/c++-10)
endif()

project(RouteKit CXX)
cmake_minimum_required(VERSION 3.16)

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

# Tests require a separate testing_data repo.  See .travis.yml.
add_custom_command(OUTPUT testing_data
    COMMAND git clone https://github.com/RoutingKit/DataForAutomaticTests.git testing_data)
add_custom_target(prepare_testing_data DEPENDS testing_data)

foreach(TEST_PROG ${TEST_SOURCES})
    get_filename_component(TEST_NAME ${TEST_PROG} NAME_WE)
    add_executable(${TEST_NAME} ${TEST_PROG})
    target_link_libraries(${TEST_NAME} ${EXE_LIBS})
endforeach()

# Match the test definitions from .travis.yml:
add_test(NAME test_bit_vector COMMAND test_bit_vector)
add_test(NAME test_buffered_asynchronous_reader COMMAND test_buffered_asynchronous_reader)
add_test(NAME test_geo_dist COMMAND test_geo_dist)
add_test(NAME test_id_mapper COMMAND test_id_mapper)
add_test(NAME test_id_set_queue COMMAND test_id_set_queue)
add_test(NAME test_inverse_vector COMMAND test_inverse_vector)
add_test(NAME test_nearest_neighbor COMMAND test_nearest_neighbor)
add_test(NAME test_nested_dissection COMMAND test_nested_dissection)
add_test(NAME test_permutation COMMAND test_permutation)
add_test(NAME test_protobuf COMMAND test_protobuf)
add_test(NAME test_sort COMMAND test_sort)
add_test(NAME test_tag_map COMMAND test_tag_map)
add_test(NAME test_strongly_connected_component COMMAND test_strongly_connected_component)
add_test(
    NAME test_customizable_contraction_hierarchy_reset
    COMMAND test_customizable_contraction_hierarchy_reset)

add_test(NAME test_osm_simple COMMAND test_osm_simple  testing_data/luxembourg.pbf)
add_dependencies(test_osm_simple  prepare_testing_data)

add_test(
    NAME test_dijkstra
    COMMAND
        test_dijkstra testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time)
add_dependencies(test_dijkstra  prepare_testing_data)

add_test(NAME test_contraction_hierarchy_extra_weight COMMAND test_contraction_hierarchy_extra_weight  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time testing_data/luxembourg_geo_distance)
add_dependencies(test_contraction_hierarchy_extra_weight  prepare_testing_data)

add_test(NAME test_contraction_hierarchy_path_query_1 COMMAND test_contraction_hierarchy_path_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time testing_data/luxembourg_travel_time_ch testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_travel_time_length)
add_dependencies(test_contraction_hierarchy_path_query  prepare_testing_data)

add_test(NAME test_contraction_hierarchy_path_query_2 COMMAND test_contraction_hierarchy_path_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_geo_distance testing_data/luxembourg_geo_distance_ch testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_geo_distance_length)

add_test(NAME test_contraction_hierarchy_pinned_query_1 COMMAND test_contraction_hierarchy_pinned_query  testing_data/luxembourg_travel_time_ch)
add_test(NAME test_contraction_hierarchy_pinned_query_2 COMMAND test_contraction_hierarchy_pinned_query  testing_data/luxembourg_geo_distance_ch)
add_dependencies(test_contraction_hierarchy_pinned_query  prepare_testing_data)

add_test(NAME test_customizable_contraction_hierarchy COMMAND test_customizable_contraction_hierarchy  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_cch_order)
add_dependencies(test_customizable_contraction_hierarchy  prepare_testing_data)

add_test(NAME test_customizable_contraction_hierarchy_path_query_1 COMMAND test_customizable_contraction_hierarchy_path_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time testing_data/luxembourg_cch_order testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_travel_time_length)
add_test(NAME test_customizable_contraction_hierarchy_path_query_2 COMMAND test_customizable_contraction_hierarchy_path_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_geo_distance testing_data/luxembourg_cch_order testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_geo_distance_length)
add_dependencies(test_customizable_contraction_hierarchy_path_query  prepare_testing_data)

add_test(NAME test_customizable_contraction_hierarchy_perfect_customization_1 COMMAND test_customizable_contraction_hierarchy_perfect_customization  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time testing_data/luxembourg_cch_order testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_travel_time_length)
add_test(NAME test_customizable_contraction_hierarchy_perfect_customization_2 COMMAND test_customizable_contraction_hierarchy_perfect_customization  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_geo_distance testing_data/luxembourg_cch_order testing_data/luxembourg_source_node testing_data/luxembourg_target_node testing_data/luxembourg_query_reference_geo_distance_length)
add_dependencies(test_customizable_contraction_hierarchy_perfect_customization  prepare_testing_data)

add_test(NAME test_customizable_contraction_hierarchy_pinned_query_1 COMMAND test_customizable_contraction_hierarchy_pinned_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_travel_time testing_data/luxembourg_cch_order)
add_test(NAME test_customizable_contraction_hierarchy_pinned_query_2 COMMAND test_customizable_contraction_hierarchy_pinned_query  testing_data/luxembourg_first_out testing_data/luxembourg_head testing_data/luxembourg_geo_distance testing_data/luxembourg_cch_order)
add_dependencies(test_customizable_contraction_hierarchy_pinned_query  prepare_testing_data)

add_test(NAME test_basic_features COMMAND test_basic_features  testing_data/luxembourg.pbf)
add_dependencies(test_basic_features  prepare_testing_data)

