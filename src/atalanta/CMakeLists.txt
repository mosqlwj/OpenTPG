cmake_minimum_required(VERSION 3.13)
project(atlanta)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. src)

add_executable(atlanta ${src} AtpgEngine.cpp AtpgEngine.h)