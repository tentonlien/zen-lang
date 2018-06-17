cmake_minimum_required(VERSION 3.5)
project(zen_virtual_machine C)

set(CMAKE_C_STANDARD 11)
#SET(CMAKE_BUILD_TYPE "Debug")
#SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
#SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(
    zen_virtual_machine
    source/main.c
    source/instructions/transfer.c
    source/instructions/arithmetic.c
    source/instructions/control.c
    source/instructions/logic.c
    source/interrupts/system.c
    source/interrupts/io.c
    header/interrupt_list.h
)