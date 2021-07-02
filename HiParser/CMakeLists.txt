cmake_minimum_required(VERSION 3.16)
project(HiParser)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/thirdparty/boost/linux/include)
include_directories(${Boost_INCLUDE_DIRS})
message("${Boost_INCLUDE_DIRS}")

add_executable(HiParser main.cpp Netlist.cpp Netlist.h StringUtils.cpp StringUtils.h Gate.cpp Gate.h Util.cpp Util.h)