cmake_minimum_required (VERSION 2.6)

project (REX C)

find_package (Threads)

add_executable(executables/rex ../src2/rex.c  ../src2/Helper.c ../src2/job.c ../src2/MyTime.c ../src2/Network.c ../src2/StringManipulator.c  ../src2/CommandsManager.c)
target_link_libraries (executables/rex ${CMAKE_THREAD_LIBS_INIT})

#[[
add_executable(executables/rexd ../src2/rexd/rexd.c ../src2/StringManipulator/StringManipulator.c ../src2/CommandsManager/CommandsManager.c ../src2/MyTime/MyTime.c ../src2/NetworkItems/job.c ../src2/CommandsManager.c)
target_link_libraries (executables/rexd ${CMAKE_THREAD_LIBS_INIT})
]]
if ( CMAKE_COMPILER_IS_GNUCC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -Wall -Wextra")
endif()
if ( MSVC )
    set(CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W4")
endif()
#[[]]
