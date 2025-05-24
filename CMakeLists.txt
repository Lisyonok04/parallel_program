cmake_minimum_required(VERSION 3.12)
project(MatrixMult)

set(CMAKE_CXX_STANDARD 17)

find_package(OpenMP REQUIRED)

add_executable(laba_2 "matrix_generator.cpp")
target_link_libraries(laba_2
    PRIVATE 
    OpenMP::OpenMP_CXX
)
target_link_libraries(laba_2 PRIVATE OpenMP::OpenMP_CXX)