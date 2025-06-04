cmake_minimum_required(VERSION 3.28)
project(TextEditor)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include/editor)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -fsanitize=address")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(BUILD_MAIN "Build just the main branch" ON)
option(BUILD_EVERYTHING "Build tests and main branch" OFF)
option(BUILD_TESTS "Build the tests" OFF)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/src")

if(BUILD_EVERYTHING)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")
endif()

if(BUILD_MAIN)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/editor")
endif()

if(BUILD_TESTS)
	add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/tests")
endif()


execute_process(COMMAND mv
		${CMAKE_BINARY_DIR}/compile_commands.json
		${CMAKE_SOURCE_DIR}/compile_commands.json
		RESULT_VARIABLE mv_result
		ERROR_VARIABLE mv_error
)

message(STATUS "Moved compile_commands.json: result=${mv_result}")
message(STATUS "mv error: ${mv_error}")
