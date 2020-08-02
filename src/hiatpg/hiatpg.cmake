#   定义了 hiatpg 的构建参数
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg" HIATPG_SOURCES)
add_executable(hiatpg ${HIATPG_SOURCES})
target_include_directories(hiatpg PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/include"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/atpg"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/netlist"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/fault"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/sim"
    "${CMAKE_CURRENT_SOURCE_DIR}/src/hiatpg/utils"
    )
