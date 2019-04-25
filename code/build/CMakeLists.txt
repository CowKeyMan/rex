cmake_minimum_required (VERSION 2.6)

project (REX C)

add_executable(executables/rex ../src/rex/rex.c ../src/linenoise/linenoise.c ../src/StringManipulator/StringManipulator.c ../src/CommandsManager/CommandsManager.c)

add_executable(executables/rexd ../src/rexd/rexd.c ../src/linenoise/linenoise.c ../src/StringManipulator/StringManipulator.c)

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()
#[[]]
