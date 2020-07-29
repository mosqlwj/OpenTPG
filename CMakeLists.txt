cmake_minimum_required(VERSION 3.4.3)
project(opentpg)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include(src/atalanta/atalanta.cmake)
#include(src/hiatpg/hiatpg.cmake)
