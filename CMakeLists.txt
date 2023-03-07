cmake_minimum_required(VERSION 3.8 FATAL_ERROR)


# ----------------------------------------------------------------------------------------------- #
#   Global CMake config
# ----------------------------------------------------------------------------------------------- #
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_EXTENSIONS OFF)

# Avoid creation of cmake_install.cmake
set(CMAKE_SKIP_INSTALL_RULES True)

# Set up configuration types for multi config generators (Visual Studio/Xcode/Ninja Multi-Config)
get_property(is_multi_config GLOBAL PROPERTY GENERATOR_IS_MULTI_CONFIG)
if(is_multi_config)
    set(CMAKE_CONFIGURATION_TYPES "Debug;Release" CACHE STRING "" FORCE)
endif()

if(NOT is_multi_config AND NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "CMAKE_BUILD_TYPE was not set - Defaulting to \"Release\"")
endif()


# ----------------------------------------------------------------------------------------------- #
#   Project config
# ----------------------------------------------------------------------------------------------- #
project(yart VERSION 0.1 LANGUAGES CXX)

# Use yart as the startup project in VS, instead of the default ALL_BUILD
set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJECT_NAME})


# ----------------------------------------------------------------------------------------------- #
#   Build tree
# ----------------------------------------------------------------------------------------------- #
# Build GLFW from source
set(GLFW_BUILD_DOCS OFF CACHE BOOL "Build GLFW without docs" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "Build GLFW without tests" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "Build GLFW without examples" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "Don't generate GLFW installation target" FORCE)

add_subdirectory(lib/glfw)

add_subdirectory(src)
