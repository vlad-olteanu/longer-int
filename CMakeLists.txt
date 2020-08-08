cmake_minimum_required(VERSION 3.16)
project(longer_int)

set(CMAKE_CXX_STANDARD 14)

add_executable(longer_int longer-int.c longer-int.h examples.c)