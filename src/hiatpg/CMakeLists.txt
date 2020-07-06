cmake_minimum_required(VERSION 3.16)
project(hiatpg)
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)



add_executable(hiatpg
    main.cpp
    utils/StringUtils.cpp
    )

target_include_directories(hiatpg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    )