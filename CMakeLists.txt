cmake_minimum_required(VERSION 3.28)
project(demo)
file(GLOB sources RELATIVE ${CMAKE_SOURCE_DIR} "src/**/*.c")
add_executable(demo src/main.c ${sources})
target_include_directories(demo PRIVATE include)
