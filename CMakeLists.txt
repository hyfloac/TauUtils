cmake_minimum_required(VERSION 3.24)
project(TauUtils VERSION 1.1.0 LANGUAGES CXX C)

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
