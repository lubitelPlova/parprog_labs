cmake_minimum_required(VERSION 3.10)
project(ImageProcessor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenCL REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(image_processor ${SOURCES})

target_link_libraries(image_processor PRIVATE
    m
    OpenCL::OpenCL
)

configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/kernel/conv_resize.cl
    ${CMAKE_CURRENT_BINARY_DIR}/conv_resize.cl
    COPYONLY
)
