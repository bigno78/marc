cmake_minimum_required(VERSION 3.10)

project(marc)

add_subdirectory(lib/stb_image)

set(SOURCES src/main.cpp src/parsing/header.cpp)

add_executable(marc ${SOURCES})

target_compile_features(marc PRIVATE cxx_std_17)
target_include_directories(marc PRIVATE src)
target_link_libraries(marc PRIVATE stb_image)

install(TARGETS marc
        RUNTIME DESTINATION bin)
