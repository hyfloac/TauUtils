cmake_minimum_required(VERSION 3.24.0)
project(TauUtils VERSION 1.0.0 LANGUAGES CXX C)

add_subdirectory(TauUtilsDynamic)
add_subdirectory(Testing)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
