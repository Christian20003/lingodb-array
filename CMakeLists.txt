cmake_minimum_required(VERSION 3.22)
project("LingoDB-Array" VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED 17)
set(CMAKE_COLOR_DIAGNOSTICS ON)

find_package(BLAS REQUIRED)

add_subdirectory(src)

add_executable(array main.cpp)

target_compile_options(array PRIVATE -Wall -Wextra -Wpedantic)

target_link_libraries(array BLAS::BLAS)
target_link_libraries(array ArrayBasics)
target_include_directories(array PUBLIC "${PROJECT_SOURCE_DIR}/include")