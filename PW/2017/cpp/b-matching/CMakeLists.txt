cmake_minimum_required (VERSION 3.1)
project (b-matching-fb332069 CXX)

find_package ( Threads REQUIRED )

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_FLAGS "-g -Wall -O3")

# http://stackoverflow.com/questions/10555706/
macro (add_executable _name)
    # invoke built-in add_executable
    _add_executable(${ARGV})
    if (TARGET ${_name})
        target_link_libraries(${_name} ${CMAKE_THREAD_LIBS_INIT} )
    endif()
endmacro()

set(SOURCE_FILES
    graph.hpp
    config.hpp

    b-adoratores.hpp
    b-adoratores.cpp

    blimit.hpp
    blimit.cpp

    main.cpp
)

add_executable(adore ${SOURCE_FILES})

