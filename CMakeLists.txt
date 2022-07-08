cmake_minimum_required(VERSION 3.10)

project(marc)

set(SOURCES src/main.cpp src/parsing/header.cpp)

add_executable(marc ${SOURCES})

target_compile_features(marc PRIVATE cxx_std_17)
target_include_directories(marc PRIVATE src)

install(TARGETS marc
        RUNTIME DESTINATION bin)
