cmake_minimum_required(VERSION 3.20)

project(Assignment2 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# OpenCV (shared with Assignment 1)
set(OpenCV_DIR "${PROJECT_SOURCE_DIR}/../external/opencv/build")

if(NOT EXISTS "${OpenCV_DIR}/OpenCVConfig.cmake")
    if(NOT EXISTS "${PROJECT_SOURCE_DIR}/../external/opencv/CMakeLists.txt")
        message(FATAL_ERROR "OpenCV sources missing. Run 'git submodule update --init --recursive' from the repository root.")
    endif()
    message(STATUS "Building shared OpenCV (first build only)")

    execute_process(
        COMMAND ${CMAKE_COMMAND}
            -S "${PROJECT_SOURCE_DIR}/../external/opencv"
            -B "${OpenCV_DIR}"
            -DCMAKE_BUILD_TYPE=Release
            -DBUILD_LIST=core,imgproc,highgui,videoio,imgcodecs,calib,geometry,features
            -DBUILD_TESTS=OFF
            -DBUILD_PERF_TESTS=OFF
            -DBUILD_EXAMPLES=OFF
            -DBUILD_opencv_apps=OFF
        COMMAND_ERROR_IS_FATAL ANY
    )

    execute_process(
        COMMAND ${CMAKE_COMMAND} --build "${OpenCV_DIR}" --parallel 8
        COMMAND_ERROR_IS_FATAL ANY
    )
endif()

find_package(OpenCV REQUIRED COMPONENTS
    core
    imgproc
    highgui
    videoio
    imgcodecs
    calib
    geometry
    features
)  
 
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
)
