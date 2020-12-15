
project("lkp")
cmake_minimum_required(VERSION 3.15)

set(LLVM_DIR /home/saim/Desktop/llvm-project/build/lib/cmake/llvm)

set(CMAKE_MODULE_PATH ${LLVM_DIR})

find_package(LLVM REQUIRED CONFIG)
list(APPEND CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR}")
include(AddLLVM)

add_definitions(${LLVM_DEFINITIONS})
include_directories(${LLVM_INCLUDE_DIRS})

add_subdirectory(pass)
