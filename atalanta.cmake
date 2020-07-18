#cmake_minimum_required(VERSION 3.13)
#project(atlanta)
#set(CMAKE_CXX_STANDARD 14)

message("atalanta.cmake: ${CMAKE_CURRENT_SOURCE_DIR}")

#   定义了 atlanta 的构架参数
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src/atalanta" ATLANTA_SOURCES)
add_executable(atalanta ${ATLANTA_SOURCES})
set_target_properties(atalanta
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin"
    )
target_include_directories(atalanta PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/atalanta"
    )

