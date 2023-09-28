cmake_minimum_required(VERSION 3.24)
project(0507mpi)

set(CMAKE_CXX_STANDARD 17)

add_executable(0507mpi main.cpp Tree.h)
set(MPI_EXEC_NUM_THREADS 3)
find_package(MPI REQUIRED)
#include_directories(${MPI_INCLUDE_PATH})             # вот без этой строчки не подключается библиотека
include_directories(/usr/lib/x86_64-linux-gnu/mpich/include/)         # эта строчка заменяет предыдущую
target_link_libraries(0507mpi ${MPI_CXX_LIBRARIES})
set(CMAKE_C_COMPILER «/usr/bin/mpicc»)
set(MPI_EXEC_NUM_THREADS 3)
#set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")