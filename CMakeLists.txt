cmake_minimum_required(VERSION 3.24)
project(TauUtils VERSION 1.4.7 LANGUAGES CXX C)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")

# Option for building shared or static library
option(GEN_TEST "Generate Test Executable" OFF)

include(CTest)

add_subdirectory(TauUtilsDynamic)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    if(${GEN_TEST})
        add_subdirectory(TauUtilsTest)
    endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_SOURCE_DIR}/bin")
