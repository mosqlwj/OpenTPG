cmake_minimum_required(VERSION 3.16)
project(hiatpg)

set(CMAKE_CXX_STANDARD 14)


set(SOURCES
    main.cpp
    )

set(HEADERS
    model.h
    cmdline.h
    LogicValue.h
    AtpgEngine.h
    )

add_executable(hiatpg ${SOURCES})