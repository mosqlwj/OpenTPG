cmake_minimum_required(VERSION 3.16)
project(hiatpg)
set(CMAKE_CXX_STANDARD 14)
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../../bin)


add_executable(hiatpg
    main.cpp
    Context.cpp
    utils/StringUtils.cpp
    include/model.h
    include/HIATPG.h
    netlist/NetListTableImpl.cpp netlist/NetListTableImpl.h fault/FaultListTableImpl.cpp fault/FaultListTableImpl.h atpg/ATPGDriverImpl.cpp atpg/ATPGDriverImpl.hpp sim/SimulateDriverImpl.cpp sim/SimulateDriverImpl.h pattern/PatternWriterImpl.cpp pattern/PatternWriterImpl.h pattern/PatternListTableImpl.cpp pattern/PatternListTableImpl.h)

target_include_directories(hiatpg PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/atpg
    ${CMAKE_CURRENT_SOURCE_DIR}/netlist
    ${CMAKE_CURRENT_SOURCE_DIR}/fault
    ${CMAKE_CURRENT_SOURCE_DIR}/sim
    ${CMAKE_CURRENT_SOURCE_DIR}/utils
    )