cmake_minimum_required(VERSION 3.28)
project(TextEditor)

find_package(Catch2 REQUIRED)

add_executable(arena_test test_arena.cpp)
target_link_libraries(arena_test allocator Catch2::Catch2WithMain)

add_executable(piecetable_test test_piecetable.cpp)
target_link_libraries(piecetable_test piece_table Catch2::Catch2WithMain)

