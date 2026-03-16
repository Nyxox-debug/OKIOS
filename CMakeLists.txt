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


FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm.git
    GIT_TAG master
)
FetchContent_MakeAvailable(glm)


add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PUBLIC external/glad/include)
if(UNIX AND NOT APPLE)
    target_link_libraries(glad PRIVATE dl)
endif()


add_executable(app
    src/main.cpp
)

target_link_libraries(app
    PRIVATE
        glad
        glfw
        OpenGL::GL   # system OpenGL
        glm::glm
)
