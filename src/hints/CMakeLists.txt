cmake_minimum_required(VERSION 3.5...3.31)
project(HintsToPot)

add_executable(hintsToPot
    HintsToPot.cpp)

target_link_libraries(hintsToPot PRIVATE boost_libs)

#encoding_check(HintsToPot)



