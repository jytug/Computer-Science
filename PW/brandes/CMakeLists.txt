cmake_minimum_required (VERSION 2.6)
project (Brandes CXX)

find_package (Threads REQUIRED)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-O2 -Wall")

set(SOURCE_FILES
    src/err.cpp
    src/err.h
    src/digraph.h
    src/brandes-sequential.cpp
    src/brandes-sequential.h
    src/brandes-parallel.cpp
    src/brandes-parallel.h
    src/in-out.cpp
    src/in-out.h
    src/brandes.cpp)

add_executable(brandes ${SOURCE_FILES})
target_link_libraries(brandes ${CMAKE_THREAD_LIBS_INIT})
