cmake_minimum_required(VERSION 3.10)

project(my-find)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
    set(CMAKE_CXX_FLAGS_RELEASE "-std=c++17 -O3")
    set(CMAKE_CXX_FLAGS_DEBUG "-std=c++17 -g -W -Wall")
else()
    message(STATUS "Use GNUCXX C++ compiler.")
endif()

include_directories(include src)

set(CMAKE_CXX_STANDARD 17)
set(EXECUTABLE_OUTPUT_PATH "${PROJECT_BINARY_DIR}/bin")

add_executable(my-find
               src/main.cpp
               src/find.cpp
               include/find.h
               src/args_wrapper.cpp
               include/args_wrapper.h
               src/dir_wrapper.cpp
               include/dir_wrapper.h)
