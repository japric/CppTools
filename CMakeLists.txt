cmake_minimum_required(VERSION 3.16)

set(PROJECT_NAME "CppTools")
set(TARGET ${PROJECT_NAME})

project(${PROJECT_NAME} VERSION 1.0.0 LANGUAGES CXX)
# C++ compiler flags
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")

add_subdirectory(Cases/FileMonitor)