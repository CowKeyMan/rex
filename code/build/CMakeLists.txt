cmake_minimum_required (VERSION 2.6)

project (REX C)

file(MAKE_DIRECTORY executables)

find_package (Threads)

add_executable(executables/rex ../src/rex.c  ../src/Helper.c ../src/job.c ../src/MyTime.c ../src/Network.c ../src/StringManipulator.c  ../src/CommandsManager.c)
target_link_libraries (executables/rex ${CMAKE_THREAD_LIBS_INIT})

add_executable(executables/rexd ../src/rexd.c  ../src/Helper.c ../src/job.c ../src/MyTime.c ../src/Network.c ../src/StringManipulator.c  ../src/CommandsManager.c)
target_link_libraries (executables/rexd ${CMAKE_THREAD_LIBS_INIT})

if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()
#[[]]
