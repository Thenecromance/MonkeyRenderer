cmake_minimum_required(VERSION 3.17)


include(../TMake/CMakeLists.txt)

set(CXXVERSION 20)

TBUILD_EXE(SandBox)

target_link_libraries(SandBox PUBLIC Core)