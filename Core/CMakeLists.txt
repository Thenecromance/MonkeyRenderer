
cmake_minimum_required(VERSION 3.17)

include(../TMake/CMakeLists.txt)

set(CXXVERSION 20)

include_directories(../deps/glfw/include)
include_directories(../deps/assimp/include)
include_directories(../deps/flecs)
include_directories(../deps/stb)
include_directories(../deps/imgui)
include_directories(../deps/spdlog/include)


TADD_SUBDIRECTORY(src)
TADD_SUBDIRECTORY(include)

TBUILD_LIB(Core)

target_link_libraries(Core PUBLIC
        flecs
        glad
        glfw
        assimp)