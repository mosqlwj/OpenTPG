# Core module sources - 核心框架层

# 插件管理
list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/plugin/PluginManager.cpp
)

# 上下文管理
list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/context/ATPGContext.cpp
)

# 内存管理
list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/memory/MemoryPool.cpp
)

# 线程管理
list(APPEND SOURCES
    ${CMAKE_CURRENT_LIST_DIR}/thread/ThreadPool.cpp
)

# 包含路径
list(APPEND INCLUDEPATH
    ${CMAKE_CURRENT_LIST_DIR}
)
