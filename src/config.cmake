# ------------------------------------------------------------------------------------------------- #
#   YART target configuration
# ------------------------------------------------------------------------------------------------- #
file(GLOB_RECURSE YART_SOURCE_FILES ${CMAKE_CURRENT_LIST_DIR}/*.cpp)
file(GLOB_RECURSE YART_HEADER_FILES ${CMAKE_CURRENT_LIST_DIR}/*.h)

add_executable(yart ${YART_SOURCE_FILES} ${YART_HEADER_FILES})
yart_set_compile_options(yart)

set_target_properties(yart PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(yart PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set_target_properties(yart PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

target_include_directories(yart PUBLIC ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(yart 
    PRIVATE Vulkan::Vulkan
    PRIVATE glfw
    PRIVATE glm
    PRIVATE imgui
    PRIVATE stb
)

# Copy dependencies into the build directory
file(COPY "src/font/codicon.ttf" DESTINATION  "${CMAKE_CURRENT_BINARY_DIR}/bin/res/fonts")
