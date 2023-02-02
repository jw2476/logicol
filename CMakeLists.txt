cmake_minimum_required(VERSION 3.15)
project(logicol)

add_subdirectory(vendor/raylib)

set(CMAKE_C_STANDARD 11)

file(GLOB_RECURSE SRC "src/*.c")

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} raylib)
