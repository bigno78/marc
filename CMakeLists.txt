cmake_minimum_required(VERSION 3.10)

project(marc)

set(SOURCES src/main.cpp src/parsing/header.cpp)
set(THREADS_PREFER_PTHREAD_FLAG ON)

find_package(Threads REQUIRED)

add_executable(marc ${SOURCES})

target_compile_features(marc PRIVATE cxx_std_17)
target_link_libraries(marc PRIVATE Threads::Threads)
