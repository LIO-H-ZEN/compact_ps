cmake_minimum_required (VERSION 2.8.11)
# check os version
if(CMAKE_SYSTEM_NAME MATCHES "Linux")
    message(STATUS "Linux supported")
else()
    message(FATAL_ERROR "not supported os")
endif()

add_definitions(-O3 --std=c++14 -Wall -Wextra -Wno-deprecated-declarations)

# include directories
include_directories(SYSTEM prepare/include utils/)

# link directories
link_directories(prepare/lib prepare/lib/.libs)

# cmake exe linker flags
set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -pthread")
set(LINK_LIB_PREFIX "")
set(LINK_LIB_SUFFIX "")

add_subdirectory(unittest)
