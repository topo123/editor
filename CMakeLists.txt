cmake_minimum_required(VERSION 3.28)
project(TextEditor)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/editor)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_library(glad ${CMAKE_CURRENT_SOURCE_DIR}/src/glad/glad.c)

find_package(glfw3 REQUIRED)
find_package(Catch2 REQUIRED)

add_executable(main "${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp")
target_link_libraries(main glfw glad)


execute_process(COMMAND mv
		${CMAKE_BINARY_DIR}/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json
		RESULT_VARIABLE mv_result
		ERROR_VARIABLE mv_error
)

message(STATUS "Moved compile_commands.json: result=${mv_result}")
message(STATUS "mv error: ${mv_error}")
