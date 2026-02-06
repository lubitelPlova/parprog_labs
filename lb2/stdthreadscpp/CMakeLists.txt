cmake_minimum_required(VERSION 3.10)
project(bin_eroder)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenCV REQUIRED)

file(GLOB SOURCES "src/*.cpp")

add_executable(bin_eroder ${SOURCES})

target_include_directories(bin_eroder PRIVATE
    ${OpenCV_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(bin_eroder PRIVATE ${OpenCV_LIBS})

