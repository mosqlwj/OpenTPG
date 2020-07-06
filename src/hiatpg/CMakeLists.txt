cmake_minimum_required(VERSION 3.16)
project(hiatpg)

set(CMAKE_CXX_STANDARD 14)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)

set(SOURCES
    main.cpp
#    Context.cpp
    utils/StringUtils.cpp )

set(HEADERS
    include/model.h
    include/Context.h
    cmdline.h
    LogicValue.h
    AtpgEngine.h
    utils/StringUtils.hpp
    )

add_executable(hiatpg ${SOURCES})
target_include_directories(hiatpg PUBLIC
    include
    utils)