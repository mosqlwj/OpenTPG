cmake_minimum_required(VERSION 3.5)

project(atalanta LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.8")
set(MACOSX_DEPLOYMENT_TARGET "10.8")

SET(SOURCES
    atalanta.cpp
    error.cpp
    hash.cpp
    help.cpp
    read_cct.cpp
    structure.cpp
    io.cpp
    pio.cpp
    stem.cpp
    define_fault_list.cpp
    testability.cpp
    fan.cpp
    learn.cpp
    ppsfp.cpp
    truth.cpp
    lsim.cpp
    fsim.cpp
    random.cpp
    print.cpp
    sim.cpp
    atalanta.cpp
    )

SET(HEADERS
    error.h
    hash.h
    help.h
    read_cct.h
    structure.h
    io.h
    pio.h
    stem.h
    define_fault_list.h
    testability.h
    fan.h
    learn.h
    ppsfp.h
    truthtable.h
    lsim.h
    fsim.h
    random.h
    print.h
    sim.h
    atalanta.h)

add_executable(atalanta SOURCES)
