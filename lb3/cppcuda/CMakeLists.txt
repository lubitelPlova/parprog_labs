cmake_minimum_required(VERSION 3.10)
project(fusion LANGUAGES CXX CUDA)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CUDA_ARCHITECTURES native)

find_package(OpenCV REQUIRED)

file(GLOB SOURCES "src/*.cpp")

add_executable(fusion ${SOURCES})

target_include_directories(fusion PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/kernels
)

add_library(kernels_lib kernels/kernels.cu)

target_link_libraries(fusion PRIVATE 
    kernels_lib
    ${OpenCV_LIBS}
)

