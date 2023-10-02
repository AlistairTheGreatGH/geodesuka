# TODO:
# Change Assimp To Static Library
# Figure out how to package libraries together in geodesuka.
cmake_minimum_required(VERSION 3.20)

# Set the C++ standard to C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(GEODESUKA_LIBRARY "geodesuka")
set(GEODESUKA_UNIT_TEST "geodesuka-unit-test")

include(FetchContent)

set(FETCHCONTENT_BASE_DIR "${CMAKE_SOURCE_DIR}/dep")

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3.8
)

# FetchContent_Declare(
#     portaudio
#     GIT_REPOSITORY https://github.com/PortAudio/portaudio.git
#     GIT_TAG        pa_stable_v190600_20161030
# )

# set(PA_BUILD_SHARED OFF)
# if (CMAKE_SYSTEM_NAME MATCHES "Windows")
#     set(PA_DLL_LINK_WITH_STATIC_RUNTIME OFF)
# endif()

FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG        master
)

FetchContent_Declare(
    assimp
    GIT_REPOSITORY https://github.com/assimp/assimp.git
    GIT_TAG        v5.2.5
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://gitlab.freedesktop.org/freetype/freetype.git
    GIT_TAG        VER-2-12-1
)

set(GLFW_BUILD_DOCS OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL " " FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(GLFW_VULKAN_STATIC ON CACHE BOOL " " FORCE)

set(BUILD_SHARED_LIBS OFF CACHE BOOL " " FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL " " FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL " " FORCE)
set(ASSIMP_INSTALL_PDB OFF CACHE BOOL " " FORCE)

FetchContent_MakeAvailable(glfw)
#FetchContent_MakeAvailable(portaudio)
FetchContent_MakeAvailable(stb)
FetchContent_MakeAvailable(assimp)
FetchContent_MakeAvailable(freetype)

# Modifies the ftoption file so it can compile while assimp uses zlib.
file(READ ${CMAKE_SOURCE_DIR}/dep/freetype-build/include/freetype/config/ftoption.h FreetypeConfig)
string(REGEX REPLACE "#define FT_CONFIG_OPTION_USE_ZLIB" "// #define FT_CONFIG_OPTION_USE_ZLIB" FreetypeConfig "${FreetypeConfig}")
file(WRITE ${CMAKE_SOURCE_DIR}/dep/freetype-build/include/freetype/config/ftoption.h "${FreetypeConfig}")

project(${GEODESUKA_LIBRARY})

# Find and include OpenMP
find_package(OpenMP REQUIRED)
if(OpenMP_CXX_FOUND)
    message("OpenMP found.")
endif()

# Find and include Vulkan
find_package(Vulkan REQUIRED)
if(Vulkan_FOUND)
    message("Vulkan found.")
    # Find the last occurrence of "/Include" in the path
    string(FIND "${Vulkan_INCLUDE_DIRS}" "/Include" REMOVE_INDEX REVERSE)

    if (REMOVE_INDEX GREATER -1)
        # Remove the "/Include" part from the path
        string(SUBSTRING "${Vulkan_INCLUDE_DIRS}" 0 ${REMOVE_INDEX} VULKAN_SDK_ROOT_DIR)
    else ()
        # If "/Include" is not found, keep the original path
        set(VULKAN_SDK_ROOT_DIR "${Vulkan_INCLUDE_DIRS}")
    endif ()

    # Print the modified path
    message(STATUS "Vulkan SDK Root Path: ${VULKAN_SDK_ROOT_DIR}")

    set(VULKAN_SDK_SECONDARY_LIBRARIES
        GenericCodeGen
        glslang-default-resource-limits
        glslang
        HLSL
        MachineIndependent
        OGLCompiler
        OSDependent
        shaderc
        shaderc_combined
        shaderc_shared
#        shaderc_util # Bug: Does Not Work On Linux!
        spirv-cross-c-shared
        spirv-cross-c
        spirv-cross-core
        spirv-cross-cpp
        spirv-cross-glsl
        spirv-cross-hlsl
        spirv-cross-msl
        spirv-cross-reflect
        spirv-cross-util
        SPIRV-Tools-diff
        SPIRV-Tools-link
        SPIRV-Tools-lint
        SPIRV-Tools-opt
        SPIRV-Tools-reduce
        SPIRV-Tools-shared
        SPIRV-Tools
        SPIRV
        SPVRemapper
    )

endif()

file(GLOB_RECURSE INC
    "inc/*.h"
)

file(GLOB_RECURSE SRC
    "src/*.h"
    "src/*.cpp"
    "src/*.c"
)

# Link against Vulkan and OpenMP
add_executable(${GEODESUKA_LIBRARY} main.cpp ${INC} ${SRC})

#set_target_properties(${GEODESUKA_LIBRARY} PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib/${CMAKE_SYSTEM_NAME}/)
set_target_properties(${GEODESUKA_LIBRARY} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin/${CMAKE_SYSTEM_NAME}/)

# Include directories
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${Vulkan_INCLUDE_DIRS}/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/inc/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/glfw-build/src/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/sdl-build/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/assimp-build/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/freetype-build/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/glfw-src/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/sdl-src/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/stb-src/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/assimp-src/include/)
target_include_directories(${GEODESUKA_LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR}/dep/freetype-src/include/)

# Link Against Vulkan
target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${Vulkan_LIBRARY})
if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/volk.lib)
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/SDL2-static.lib)
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/SDL2.lib)
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/SDL2main.lib)
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/SDL2_test.lib)
    target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE ${VULKAN_SDK_ROOT_DIR}/Lib/dxcompiler.lib)
    foreach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
        target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE debug ${VULKAN_SDK_ROOT_DIR}/Lib/${lib}d.lib)
    endforeach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
    foreach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
        target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE optimized ${VULKAN_SDK_ROOT_DIR}/Lib/${lib}.lib)
    endforeach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -lvolk)
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -lSDL2-static)
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -lSDL2)
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -lSDL2main)
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -lSDL2_test)
    # target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE -ldxcompiler)
    foreach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
        target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE debug -l${lib}d)
    endforeach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
    foreach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
        target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE optimized -l${lib})
    endforeach(lib ${VULKAN_SDK_SECONDARY_LIBRARIES})
elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
else()
endif()

# Link Third Party Libraries
target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE glfw)
target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE assimp)
target_link_libraries(${GEODESUKA_LIBRARY} PRIVATE freetype)
