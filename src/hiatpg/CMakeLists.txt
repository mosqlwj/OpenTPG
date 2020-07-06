cmake_minimum_required(VERSION 3.16)
project(hiatpg)

set(CMAKE_CXX_STANDARD 14)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)

set(SOURCES
    main.cpp
#    Context.cpp
    )

set(HEADERS
    model.h
    cmdline.h
    LogicValue.h
    AtpgEngine.h
    Context.h
    )

add_executable(hiatpg ${SOURCES})