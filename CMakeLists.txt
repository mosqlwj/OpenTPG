cmake_minimum_required(VERSION 3.4.3)
project(opentpg)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

set(CMAKE_CXX_FLAGS "-fsigned-char ${CMAKE_CXX_FLAGS}")

include(src/atalanta/atalanta.cmake)
