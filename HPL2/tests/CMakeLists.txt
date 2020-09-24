cmake_minimum_required (VERSION 2.8.11)
project(HPL2)

file(GLOB all_sources RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
    src/Common/*.cpp
    src/Common/*.h
)
