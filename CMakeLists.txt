cmake_minimum_required(VERSION 3.16)
project(OKIOS LANGUAGES C CXX)

include(FetchContent)

find_package(OpenGL REQUIRED)


FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glfw)

# Vendors
add_subdirectory(vendors/glm)


add_library(glad STATIC vendors/glad/src/glad.c)
target_include_directories(glad PUBLIC vendors/glad/include)
if(UNIX AND NOT APPLE)
    target_link_libraries(glad PRIVATE dl)
endif()


add_executable(engine
    src/main.cpp
    src/engine.cpp
    src/mesh.cpp
    src/shader/shader.cpp
    src/camera.cpp
    src/terrain/terrain.cpp
    src/transform.cpp
    src/world.cpp
)

target_link_libraries(engine
    PRIVATE
        glad
        glfw
        OpenGL::GL   # system OpenGL
        glm::glm
)

target_include_directories(engine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/vendors
)
