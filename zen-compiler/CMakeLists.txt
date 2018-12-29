cmake_minimum_required(VERSION 3.5)
project(zen-compiler)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

add_executable(
    zenc
    zenc.cpp
    lexer.cpp
    parser.cpp
    generator.cpp
    assembler.cpp
)