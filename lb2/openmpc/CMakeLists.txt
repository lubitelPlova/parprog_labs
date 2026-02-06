cmake_minimum_required(VERSION 3.10)
project(ImageProcessor C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

find_package(OpenMP REQUIRED)

file(GLOB SOURCES "src/*.c")

add_executable(image_processor ${SOURCES})

target_link_libraries(image_processor PRIVATE
    OpenMP::OpenMP_C
    m
)

target_compile_options(image_processor PRIVATE
    -g
    -fopenmp
)
