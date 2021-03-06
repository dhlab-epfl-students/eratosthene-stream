cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 17)
find_package(glfw3 3.2 REQUIRED)
find_package(Vulkan REQUIRED)

# include head-files libraries
include_directories("code/include")
add_subdirectory("code/include/IXWebSocket")

# pre compile shaders
set(GLSL_COMPILER glslc)

set(RESOURCES
        ${CMAKE_SOURCE_DIR}/code/shaders/shader.vert
        ${CMAKE_SOURCE_DIR}/code/shaders/shader.frag
        )

set(COMPILED_RESOURCES
        ${CMAKE_SOURCE_DIR}/code/shaders/shader.vert.spv
        ${CMAKE_SOURCE_DIR}/code/shaders/shader.frag.spv
        )

set(HEADERS
        code/src/engine.h
        code/src/models.h
        code/src/utils.h
        code/src/server.h
        )

set(EXTERNAL_HEADERS
        code/include/stb/stb_image.h
        code/include/stb/stb_image_write.h
        code/include/base64/base64.h
        code/include/base64/base64.cpp
        code/include/nlohmann/json.hpp
        code/include/happly/happly.h
        )

set(SOURCES
        code/src/engine.cpp
        code/src/server.cpp
        code/src/utils.cpp)


foreach (RESOURCE ${RESOURCES})
    add_custom_command(OUTPUT ${RESOURCE}.spv
            COMMAND ${GLSL_COMPILER} ${RESOURCE} -o ${RESOURCE}.spv
            DEPENDS ${RESOURCE}
            COMMENT "Rebuilding ${RESOURCE}.spv")
    message(STATUS "Generating build commands for ${RESOURCE}.spv")
endforeach ()

add_executable(
        eratosthene-stream
        ${EXTERNAL_HEADERS}
        ${HEADERS}
        ${SOURCES}
        ${RESOURCES}
        ${COMPILED_RESOURCES}
)

target_link_libraries(eratosthene-stream glfw ${GLFW_LIBRARIES} Vulkan::Vulkan ixwebsocket)