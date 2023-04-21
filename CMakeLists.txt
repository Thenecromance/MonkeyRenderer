cmake_minimum_required(VERSION 3.17)

include(TMake/CMakeLists.txt)

#set c++ version to C++ 20
set(CXXVERSION 20)

TRunBootstrap()
include_directories(deps/glad/include)
include_directories(deps/stb)
include_directories(deps/glm)
include_directories(deps/flecs/include)

set(GLAD_SOURCES deps/glad/src/glad.c)
add_library(glad ${GLAD_SOURCES})

set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "")
set(GLFW_BUILD_TESTS OFF CACHE BOOL "")
set(GLFW_BUILD_DOCS OFF CACHE BOOL "")
set(GLFW_INSTALL OFF CACHE BOOL "")


function(set_up_and_import_assimp)
    # import assimp
    set(BUILD_SHARED_LIBS ON CACHE BOOL "")
    set(ASSIMP_NO_EXPORT ON CACHE BOOL "")
    set(ASSIMP_BUILD_DRACO OFF CACHE BOOL "")
    set(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "")
    set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "")
    set(ASSIMP_INSTALL_PDB OFF CACHE BOOL "")
    set(ASSIMP_BUILD_ALL_IMPORTERS_BY_DEFAULT OFF CACHE BOOL "")
    set(ASSIMP_BUILD_OBJ_IMPORTER ON CACHE BOOL "")
    set(ASSIMP_BUILD_GLTF_IMPORTER ON CACHE BOOL "")
    add_subdirectory(deps/assimp)
endfunction(set_up_and_import_assimp)
set_up_and_import_assimp()

# add_subdirectory(deps/taskflow)
add_subdirectory(deps/glfw)
add_subdirectory(deps/crossguid)
add_subdirectory(deps/flecs)
add_subdirectory(deps/spdlog)

add_subdirectory(Core)

add_subdirectory(Tools)
add_subdirectory(Experimental)

include_directories(Core/include)

add_subdirectory(SandBox)
# include(CPack)

