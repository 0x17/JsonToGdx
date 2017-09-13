cmake_minimum_required(VERSION 3.8)
project(JsonToGdx)

#set(CMAKE_VERBOSE_MAKEFILE 1)

if(APPLE)
    set(CMAKE_C_COMPILER, gcc-7)
    set(CMAKE_CXX_COMPILER, gcc-7)
    set(GAMS_PATH /Applications/GAMS24.9/sysdir)
else()
    set(GAMS_PATH /home/andre/Downloads/gams24.9_linux_x64_64_sfx)
endif()

set(CMAKE_CXX_STANDARD 14)

include_directories(${GAMS_PATH}/apifiles/C/api ${GAMS_PATH}/apifiles/C++/api)
link_directories(${GAMS_PATH})

set(SHARED_SOURCES json11.cpp json11.hpp jsontogdx.cpp jsontogdx.h)

set(SOURCE_FILES main.cpp ${SHARED_SOURCES})
add_executable(JsonToGdx ${SOURCE_FILES})
target_link_libraries(JsonToGdx gamscpp pthread)

add_library(JsonToGdxLib SHARED ${SHARED_SOURCES})
target_link_libraries(JsonToGdxLib gamscpp pthread)

