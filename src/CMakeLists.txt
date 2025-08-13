find_package(glfw3 REQUIRED)

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c)
add_library(allocator ${CMAKE_CURRENT_SOURCE_DIR}/arenas/BumpAllocator.cpp)
add_library(piece_table ${CMAKE_CURRENT_SOURCE_DIR}/piece/PieceTable.cpp)
add_library(renderer ${CMAKE_CURRENT_SOURCE_DIR}/renderer/renderer.cpp)

target_link_libraries(piece_table allocator)
target_link_libraries(renderer freetype allocator piece_table)
