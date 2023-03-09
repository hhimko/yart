cmake_minimum_required(VERSION 3.13 FATAL_ERROR)


# ------------------------------------------------------------------------------------------------- #
#   Global CMake config
# ------------------------------------------------------------------------------------------------- #
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Avoid creation of cmake_install.cmake
set(CMAKE_SKIP_INSTALL_RULES True)

# Set up configuration types for multi config generators (Visual Studio\Xcode\Ninja Multi-Config)
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release;Profile" CACHE STRING "" FORCE)
endif()

if(NOT is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "CMAKE_BUILD_TYPE was not set - Defaulting to \"Release\"")
endif()

# Sets common compile options for a target 
function(yart_set_compile_options target)
    # --- Common --- #
    target_compile_options(${target} PRIVATE -Wall)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        # Suppress MSVC compiler warning C4464 (relative include path contains '..')
        target_compile_options(${target} PRIVATE /wd4464)
    endif()

    # --- Debug --- #
    target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-DYART_DEBUG>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:/Zi>)
    else()
        target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-Og>) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-g3>) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-Wextra>)
        target_compile_options(${target} PRIVATE $<$<CONFIG:Debug>:-pendantic>)
    endif()

    # --- Release --- #
    target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-DNDEBUG>)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:/O2>) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:/Ob2>) 
        # Open Release builds without console
        target_link_options(${target} PRIVATE $<$<CONFIG:Release>:/SUBSYSTEM:WINDOWS>)
        target_link_options(${target} PRIVATE $<$<CONFIG:Release>:/ENTRY:mainCRTStartup>)
        # Suppress MSVC compiler warning C4710 (function not inlined) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:/wd4710>)
    else()
        target_compile_options(${target} PRIVATE $<$<CONFIG:Release>:-Ofast>)
    endif()

    # --- Profile --- #
    target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:-DNDEBUG>) 
    if(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:/O2>) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:/Ob2>) 
        target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:/Zi>) 
    else()
        target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:-Ofast>)
        target_compile_options(${target} PRIVATE $<$<CONFIG:Profile>:-g3>)
    endif()
endfunction()


# ------------------------------------------------------------------------------------------------- #
#   Project config
# ------------------------------------------------------------------------------------------------- #
project(yart VERSION 0.1 LANGUAGES CXX)

# Use yart as the startup project in VS, instead of the default ALL_BUILD
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


# ------------------------------------------------------------------------------------------------- #
#   Build tree
# ------------------------------------------------------------------------------------------------- #
function(check_submodule_exists name)
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/lib/${name}/CMakeLists.txt")
        message(FATAL_ERROR 
            "${name} missing from lib directory! "
            "Make sure to clone with flag --recurse-submodules "
            "or run git submodule update --init --recursive"
        )
    endif()
endfunction()

# Build GLFW from source
check_submodule_exists(glfw)
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Don't generate GLFW installation target" FORCE)
add_subdirectory(lib/glfw EXCLUDE_FROM_ALL)

# Add GLM header-only library
check_submodule_exists(glm)
add_library(glm INTERFACE)
target_include_directories(glm INTERFACE lib/glm)

add_subdirectory(src)
