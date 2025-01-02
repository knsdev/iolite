cmake_minimum_required(VERSION 3.16)

project(iolite)

add_compile_options("$<$<CONFIG:Debug>:-DIOL_DEBUG>")
add_compile_options("$<$<CONFIG:Release>:-DIOL_RELEASE>")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)

# Define a common output directory for all binaries
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Include directories for third-party libraries
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/glew/include)
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/SDL2/include)

# Add subdirectories for the engine and game
add_subdirectory(engine)
add_subdirectory(game)

