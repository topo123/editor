cmake_minimum_required(VERSION 3.28)
project(TextEditor)

find_package(Catch2 REQUIRED)

add_executable(test tests.cpp)
target_link_libraries(test allocator Catch2::Catch2WithMain)

