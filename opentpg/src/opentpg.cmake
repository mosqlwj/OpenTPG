list(APPEND INCLUDEPATH ${CMAKE_CURRENT_LIST_DIR})

include(${CMAKE_CURRENT_LIST_DIR}/opentpg-conf.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/opentpg-deps.cmake)

include(${CMAKE_CURRENT_LIST_DIR}/adl/adl.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/core/core.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/flowtpg/flowtpg.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/logger/logger.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/shell/shell.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/sim/sim.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/utils/utils.cmake)

