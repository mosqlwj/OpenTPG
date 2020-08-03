#   定义了 atalanta 的构建参数
aux_source_directory("${CMAKE_CURRENT_SOURCE_DIR}/src/atalanta" ATLANTA_SOURCES)
add_executable(atalanta ${ATLANTA_SOURCES})
target_include_directories(atalanta PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/src/atalanta"
    )

