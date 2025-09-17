cmake_minimum_required(VERSION 3.23)
project(TauUtils VERSION 1.4.6 LANGUAGES CXX C)

# Option for building shared or static library
option(BUILD_SHARED_LIBS "Build shared libraries" OFF)

message("CMake Source Dir: ${CMAKE_SOURCE_DIR}")
message("CMake Current Source Dir: ${CMAKE_CURRENT_SOURCE_DIR}")

# We use this to check for some compiler flags, mostly to disable warnings.
include(CheckCCompilerFlag)
# This is a helper utility for generating the folder layout in VS.
include(GenVsFilters)
include(SetCompileFlags)

file(GLOB_RECURSE SOURCES "src/*.cpp" "src/*.c" "src/*.rc")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h" "include/*.inl")

# Setup the Dynamic and Static libraries with the source files.
add_library(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Generate the "filters" for VS. This basically just creates an intuitive folder layout.
GenVsFilters(SOURCES)
GenVsFilters(HEADERS)

target_sources(${PROJECT_NAME} PRIVATE ${SOURCES})
target_sources(${PROJECT_NAME} PUBLIC FILE_SET "HEADERS" BASE_DIRS "include" FILES ${HEADERS})

if(NOT ${NO_GEN_TEST})
#    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:TauUtilsDynamicShared> $<TARGET_FILE_DIR:TauUtilsTest>)
endif()

# Set the include directory.
target_include_directories(${PROJECT_NAME} PUBLIC include)
# Set the source directory.
target_include_directories(${PROJECT_NAME} PRIVATE src)

SetCompileFlags(${PROJECT_NAME} PUBLIC PRIVATE ${BUILD_SHARED_LIBS})

if(BUILD_SHARED_LIBS)
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DTAU_UTILS_BUILD_SHARED)
else()
    target_compile_definitions(${PROJECT_NAME} PRIVATE -DTAU_UTILS_BUILD_STATIC)
endif()

install(
    TARGETS ${PROJECT_NAME}
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FILE_SET HEADERS
)

add_library(TauUtils::TauUtils ALIAS ${PROJECT_NAME})
