cmake_minimum_required(VERSION 3.20)

project(Assignment1 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --------------------------------------------------
# OpenCV

set(OpenCV_DIR "${PROJECT_SOURCE_DIR}/external/opencv/build")

find_package(OpenCV REQUIRED COMPONENTS
    core
    imgproc
    highgui
    videoio
)

# --------------------------------------------------
# GLFW

set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)

add_subdirectory(external/glfw)

# --------------------------------------------------
# GLAD

add_subdirectory(external/glad/cmake glad_cmake)

glad_add_library(
    glad
    STATIC
    REPRODUCIBLE
    API gl:core=3.3
)

# --------------------------------------------------
# Project files

file(GLOB_RECURSE PROJECT_SOURCES CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/src/*.cpp"
)

file(GLOB_RECURSE PROJECT_HEADERS CONFIGURE_DEPENDS
    "${PROJECT_SOURCE_DIR}/include/*.hpp"
    "${PROJECT_SOURCE_DIR}/include/*.h"
)

add_executable(${PROJECT_NAME}
    ${PROJECT_SOURCES}
    ${PROJECT_HEADERS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${PROJECT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
    ${OpenCV_LIBS}
    glfw
    glad
)