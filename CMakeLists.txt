cmake_minimum_required(VERSION 2.8.12)
project(labynkyr CXX)

# Compiler flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -pedantic -std=c++11 -msse4.2 -msse2 -msse -march=native -maes")

# -pthread
find_package(Threads)

# Get the main source and headers
file(GLOB_RECURSE src_all_headers_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/*.hpp)
file(GLOB_RECURSE src_all_sources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/*.cpp)
source_group("" FILES ${src_all_headers_} ${src_all_sources_})

# Get the test source and headers
file(GLOB_RECURSE test_headers_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.hpp)
file(GLOB_RECURSE test_sources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} test/*.cpp)
source_group("" FILES ${test_headers_} ${test_sources_})

# Get the source and headers for examples
file(GLOB_RECURSE example_headers_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} examples/*.hpp)
file(GLOB_RECURSE example_sources_ RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} examples/*.cpp)
source_group("" FILES ${example_headers_} ${example_sources_})

# Include directories
include_directories(RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/ examples/)

# Create unit test executable
add_executable(test-labynkyr ${test_headers_} ${test_sources_})
target_link_libraries(test-labynkyr unittest++ ${CMAKE_THREAD_LIBS_INIT})

# Run unit tests as post build step
add_custom_command(TARGET test-labynkyr POST_BUILD COMMAND test-labynkyr COMMENT "Running unit tests")

# Create examples executable
add_executable(examples ${example_headers_} ${example_sources_})
target_link_libraries(examples ${CMAKE_THREAD_LIBS_INIT})

# Installation
set (CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_SOURCE_DIR})
set (LABYNKYR_INCLUDE_DESTINATION "include/labynkyr/")
set (LABYNKYR_INSTALL_DESTINATION "bin/")

# Install examples executable
install(TARGETS examples DESTINATION "${LABYNKYR_INSTALL_DESTINATION}")

# Install headers to include/
file(GLOB src_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/*.hpp)
file(GLOB rank_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/rank/*.hpp)
file(GLOB search_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/search/*.hpp)
file(GLOB search_enumerate_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/search/enumerate/*.hpp)
file(GLOB search_parallel_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/search/parallel/*.hpp)
file(GLOB search_verify_headers RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} src/labynkyr/search/verify/*.hpp)

install(FILES ${src_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/")
install(FILES ${rank_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/rank/")
install(FILES ${search_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/search/")
install(FILES ${search_enumerate_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/search/enumerate/")
install(FILES ${search_parallel_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/search/parallel/")
install(FILES ${search_verify_headers} DESTINATION "${LABYNKYR_INCLUDE_DESTINATION}/search/verify/")
