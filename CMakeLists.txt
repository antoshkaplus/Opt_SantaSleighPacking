cmake_minimum_required(VERSION 3.7)
project(include)

set(CMAKE_CXX_STANDARD 11)


set(SOURCE_FILES
        app/main_check.cpp
        include/BaseLayer.h
        src/Help.cpp
        include/Help.h
        app/main.cpp
        src/MaxRectPacking.cpp
        include/MaxRectPacking.h
        include/Packing.h
        include/Shelf.h
        include/TailLayer.h)

add_executable(include ${SOURCE_FILES})