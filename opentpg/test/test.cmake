# 测试源文件列表配置
# 组织测试代码的源文件和头文件

# 初始化测试源文件和头文件列表
set(TEST_SOURCES "")
set(TEST_HEADERS "")

# 获取测试目录路径
set(TEST_DIR ${CMAKE_CURRENT_LIST_DIR})

# 测试主入口文件
list(APPEND TEST_SOURCES ${TEST_DIR}/test_main.cpp)

# 包含各个模块的测试配置
include(${TEST_DIR}/model/test_model.cmake)
include(${TEST_DIR}/logger/test_logger.cmake)
