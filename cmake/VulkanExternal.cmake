# ------------------------------------------------------------------------------------------------- #
#   Helper module for finding the Vulkan SDK
# ------------------------------------------------------------------------------------------------- #
set(VULKAN_SDK "C:/VulkanSDK")
set(VULKAN_VERSION "")


macro(_list_subdirs result dir)
    file(GLOB _subdirs LIST_DIRECTORIES TRUE ${dir}/*)
    foreach(_subdir ${_subdirs})
        if(IS_DIRECTORY ${_subdir})
            list(APPEND ${result} ${_subdir})
        endif()
    endforeach()
endmacro()


macro(_include_vulkan_from_path path)
    set(Vulkan_DIR ${path})
    find_library(Vulkan_LIBRARY NAMES vulkan-1 vulkan PATHS "${Vulkan_DIR}/Lib")
    set(Vulkan_INCLUDE_DIR "${Vulkan_DIR}/Include")

    find_package(Vulkan QUIET)
endmacro()


macro(find_vulkan)
    message(STATUS "Including Vulkan SDK")

    # Use builtin module
    find_package(Vulkan QUIET)

    if(WIN32 AND NOT Vulkan_FOUND)
        # Fall back to looking for Vulkan manually 
        message(STATUS "Failed to find Vulkan SDK using CMake. Retrying to locate it manually")

        if(EXISTS ${VULKAN_SDK})
            if(VULKAN_VERSION STREQUAL "")
                _list_subdirs(subdirs "${VULKAN_SDK}")
                if(CMAKE_VERSION VERSION_GREATER_EQUAL 3.18)
                    list(SORT subdirs COMPARE NATURAL ORDER DESCENDING)
                endif()

                foreach(path ${subdirs})
                    _include_vulkan_from_path(${path})
                    if(${Vulkan_FOUND})
                        string(REPLACE "${VULKAN_SDK}/" "" version ${path})
                        message(STATUS "Successfully found Vulkan SDK version ${version}")
                        break()
                    endif()
                endforeach()

            elseif(EXISTS "${VULKAN_SDK}/${VULKAN_VERSION}")
                _include_vulkan_from_path("${VULKAN_SDK}/${VULKAN_VERSION}")
                if(${Vulkan_FOUND})
                    message(STATUS "Successfully found Vulkan SDK version ${VULKAN_VERSION}")
                endif()
            else()
                message(SEND_ERROR "Unable to locate Vulkan SDK version given by VULKAN_VERSION: ${VULKAN_VERSION}")
            endif() # if(VULKAN_VERSION STREQUAL "")

        else()
            message(SEND_ERROR "Unable to locate Vulkan SDK path given by VULKAN_SDK: ${VULKAN_SDK}")
        endif() # if(EXISTS ${VULKAN_SDK})
        
    endif() # if(WIN32 AND NOT Vulkan_FOUND)
endmacro()
