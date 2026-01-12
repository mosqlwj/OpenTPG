# Logger 模块单元测试配置
# 测试日志系统的功能

# Logger 单元测试
list(APPEND TEST_SOURCES ${TEST_DIR}/logger/TestLogger.cpp)

# 包含所需的源文件
list(APPEND TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/logger/Logger.cpp)
