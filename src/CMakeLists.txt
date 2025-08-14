find_package(glfw3 REQUIRED)

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/glad/glad.c)
add_library(allocator ${CMAKE_CURRENT_SOURCE_DIR}/arenas/BumpAllocator.cpp)
add_library(piece_table ${CMAKE_CURRENT_SOURCE_DIR}/piece/PieceTable.cpp)
add_library(renderer ${CMAKE_CURRENT_SOURCE_DIR}/renderer/renderer.cpp)
add_library(type_buffer ${CMAKE_CURRENT_SOURCE_DIR}/type_buffer/TypeBuffer.cpp)
add_library(input_events ${CMAKE_CURRENT_SOURCE_DIR}/input/InputHandler.cpp)

target_link_libraries(piece_table allocator)
target_link_libraries(renderer freetype allocator piece_table type_buffer)
