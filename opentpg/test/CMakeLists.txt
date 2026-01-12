# OpenTPG 测试模块 CMake 配置
# 使用 Google Test 框架进行单元测试

cmake_minimum_required(VERSION 3.14)

project(opentpg_tests)

# 设置 C++11 标准
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 构建测试的选项
option(BUILD_TESTS "Build the tests" ON)

if(NOT BUILD_TESTS)
    return()
endif()

# 启用测试
enable_testing()

# 使用 FetchContent 下载 Google Test
include(FetchContent)

FetchContent_Declare(
    googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG release-1.11.0
)

# 配置 Google Test
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

# 包含测试源文件配置
include(${CMAKE_CURRENT_SOURCE_DIR}/test.cmake)

# 创建测试可执行文件
add_executable(opentpg_test ${TEST_SOURCES} ${TEST_HEADERS})

# 设置包含目录
target_include_directories(opentpg_test PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/src/flowtpg
)

# 链接库
target_link_libraries(opentpg_test
    PRIVATE
    gtest
    gmock
    pthread
)

# 自动发现并添加测试
include(GoogleTest)
gtest_discover_tests(opentpg_test)
