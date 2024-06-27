cmake_minimum_required(VERSION 3.28)
project(bmp_encode)

set(CMAKE_CXX_STANDARD 14)

# added -fopenmp
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fopenmp")

include_directories(.)

add_executable(bmp_encode_openmp
        Fano.cpp
        RLE.cpp
        bmp_read.cpp
        openmp.cpp
)
add_executable(bmp_encode_main
        Fano.cpp
        RLE.cpp
        bmp_read.cpp
        main.cpp
)

find_package(MPI REQUIRED)
target_link_libraries(bmp_encode_main PUBLIC MPI::MPI_CXX)

