cmake_minimum_required(VERSION 3.15)
project(OPENTPG)

#   加载构建模板
include(template.cmake)

#   引入基本定义和源文件
include(${CMAKE_CURRENT_LIST_DIR}/src/opentpg.cmake)

#   定义 opentpg 可执行文件及其关键参数
add_executable(opentpg "src/main.cpp")

#   根据 Debug/Release 模式配置不同的编译选项
if (${CMAKE_BUILD_TYPE} MATCHES "Debug")
    # Debug 模式配置
    target_sources(opentpg PRIVATE ${SOURCES} ${SOURCES_DEBUG})
    target_include_directories(opentpg PRIVATE ${INCLUDEPATH} ${INCLUDEPATH_DEBUG})
    target_compile_definitions(opentpg PRIVATE ${DEFINES} ${DEFINES_DEBUG})
    target_compile_options(opentpg PRIVATE ${CXXFLAGS} ${CXXFLAGS_DEBUG})
    target_compile_options(opentpg PRIVATE ${CFLAGS} ${CFLAGS_DEBUG})
    target_link_options(opentpg PRIVATE ${LIBS} ${LIBS_DEBUG})
    target_link_options(opentpg PRIVATE ${LFLAGS} ${LFLAGS_DEBUG})
else ()
    # Release 模式配置
    target_sources(opentpg PRIVATE ${SOURCES} ${SOURCES_RELEASE})
    target_include_directories(opentpg PRIVATE ${INCLUDEPATH} ${INCLUDEPATH_RELEASE})
    target_compile_definitions(opentpg PRIVATE ${DEFINES} ${DEFINES_RELEASE})
    target_compile_options(opentpg PRIVATE ${CXXFLAGS} ${CXXFLAGS_RELEASE})
    target_compile_options(opentpg PRIVATE ${CFLAGS} ${CFLAGS_RELEASE})
    target_link_options(opentpg PRIVATE ${LIBS} ${LIBS_RELEASE})
    target_link_options(opentpg PRIVATE ${LFLAGS} ${LFLAGS_RELEASE})
endif ()

#   测试模块配置
option(BUILD_TESTS "Build tests" ON)
if(BUILD_TESTS)
    add_subdirectory(test)
endif()
