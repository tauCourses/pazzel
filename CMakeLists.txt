cmake_minimum_required(VERSION 3.8)
project(pazzel)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES main.cpp parsedData.h parsedData.cpp)
add_executable(pazzel ${SOURCE_FILES})