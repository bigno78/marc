add_library(stb_image src/stb_image_write.cpp)
target_include_directories(stb_image PUBLIC ${CMAKE_CURRENT_LIST_DIR}/include)
