cmake_minimum_required(VERSION 3.12)
project(HiAtpg)

set(CMAKE_CXX_STANDARD 11)

set(Boost_INCLUDE_DIRS thirdparty/boost/linux/include)
set(Cmdline_INCLUDE_DIRS thirdparty/cmdline/linux/include)
include_directories(${Boost_INCLUDE_DIRS})
message("${Boost_INCLUDE_DIRS}")
include_directories(${Cmdline_INCLUDE_DIRS})

aux_source_directory(src SRC)
add_executable(HiAtpg ${SRC})