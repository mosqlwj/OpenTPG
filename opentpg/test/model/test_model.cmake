# Model 模块单元测试配置
# 测试 Gate、Fault、TestCube 等核心数据模型

# Gate 单元测试
list(APPEND TEST_SOURCES ${TEST_DIR}/model/TestGate.cpp)

# Fault 单元测试
list(APPEND TEST_SOURCES ${TEST_DIR}/model/TestFault.cpp)

# TestCube 单元测试
list(APPEND TEST_SOURCES ${TEST_DIR}/model/TestTestCube.cpp)

# 包含所需的源文件
list(APPEND TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/flowtpg/model/Gate.cpp)
list(APPEND TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/flowtpg/model/Fault.cpp)
list(APPEND TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/flowtpg/model/Util.cpp)
list(APPEND TEST_SOURCES ${CMAKE_SOURCE_DIR}/src/utils/strings.cpp)
