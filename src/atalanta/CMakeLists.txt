cmake_minimum_required(VERSION 3.13)
project(atalanta)

set(CMAKE_CXX_STANDARD 14)

aux_source_directory(. src)

add_executable(atalanta ${src} AtpgEngine.cpp AtpgEngine.h)