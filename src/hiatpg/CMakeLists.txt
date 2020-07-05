cmake_minimum_required(VERSION 3.16)
project(hiatpg)

set(CMAKE_CXX_STANDARD 14)

add_executable(hiatpg main.cpp model.h cmdline.h LogicValue.h)