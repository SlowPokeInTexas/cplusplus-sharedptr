cmake_minimum_required(VERSION 4.3)
project(testit)

set(CMAKE_CXX_STANDARD 26)

# Enable ThreadSanitizer for Clang/GCC
add_compile_options(-fsanitize=thread -g -O1)
add_link_options(-fsanitize=thread)

add_executable(testit main.cpp)
