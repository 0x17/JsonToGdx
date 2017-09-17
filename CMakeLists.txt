cmake_minimum_required(VERSION 3.8)
project(JsonToGdx)

#set(CMAKE_VERBOSE_MAKEFILE 1)

find_package(Boost REQUIRED)

if(APPLE)
#    set(CMAKE_C_COMPILER, gcc-7)
#    set(CMAKE_CXX_COMPILER, gcc-7)
    set(GAMS_PATH /Applications/GAMS24.9/sysdir)
else()
    set(GAMS_PATH /home/andre/Downloads/gams24.9_linux_x64_64_sfx)
endif()

set(CMAKE_CXX_STANDARD 14)

include_directories(${GAMS_PATH}/apifiles/C/api ${GAMS_PATH}/apifiles/C++/api ${Boost_INCLUDE_DIRS})
link_directories(${GAMS_PATH})

set(EXAMPLE_SOURCES examples.cpp examples.hpp)
set(SHARED_SOURCES json11.cpp json11.hpp jsontogdx.cpp jsontogdx.h utils.cpp utils.hpp)

set(SOURCE_FILES main.cpp ${SHARED_SOURCES} ${EXAMPLE_SOURCES})
add_executable(JsonToGdx ${SOURCE_FILES})
target_link_libraries(JsonToGdx gamscpp pthread ${Boost_LIBRARIES})

add_library(JsonToGdxLib SHARED ${SHARED_SOURCES})
target_link_libraries(JsonToGdxLib gamscpp pthread ${Boost_LIBRARIES})

