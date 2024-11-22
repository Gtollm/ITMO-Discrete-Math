cmake_minimum_required(VERSION 3.10)
project(MyProject)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD_REQUIRED True)

include_directories(src include tests)

file(GLOB SOURCES "src/*.cpp")

add_executable(MyProject ${SOURCES})

enable_testing()
add_subdirectory(tests)