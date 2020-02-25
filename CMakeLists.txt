cmake_minimum_required(VERSION 3.5)
project(jackmono)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)
find_package(Jack)

include_directories(${JACK_INCLUDE_DIR})

add_executable(wildcard_test "wildcard.c" "wildcard.h" "wc.c")
add_executable(jackm "jackm.c" "wildcard.c" "wildcard.h" "stack.c" "stack.h" "util.h" "util.c")
target_link_libraries(jackm ${JACK_LIBRARIES})
