#cmake_minimum_required(VERSION 3.13)
#project(hiatpg)
#set(CMAKE_CXX_STANDARD 14)


#   定义了 hiatpg 的构架参数
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg" HIATPG_SOURCES)
add_executable(hiatpg ${HIATPG_SOURCES})
set_target_properties(hiatpg
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    )
target_include_directories(hiatpg PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/atpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/netlist"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/fault"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/sim"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/utils"
    )
