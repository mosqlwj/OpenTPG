cmake_minimum_required(VERSION 3.16)
project(HiAtpg)

set(CMAKE_CXX_STANDARD 14)

set(Boost_INCLUDE_DIRS thirdparty/boost/linux/include)
set(Cmdline_INCLUDE_DIRS thirdparty/cmdline/linux/include)
include_directories(${Boost_INCLUDE_DIRS})
message("${Boost_INCLUDE_DIRS}")
include_directories(${Cmdline_INCLUDE_DIRS})

add_executable(HiAtpg src/main.cpp src/Netlist.cpp src/Netlist.h src/Gate.cpp src/Gate.h src/Util.cpp src/Util.h src/Params.cpp src/Params.h src/Fault.cpp src/Fault.h)