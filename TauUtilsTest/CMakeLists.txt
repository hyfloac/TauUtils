cmake_minimum_required(VERSION 3.24)
project(TauUtilsTest LANGUAGES CXX C)

# We use this to check for some compiler flags, mostly to disable warnings.
include(CheckCCompilerFlag)
# This is a helper utility for generating the folder layout in VS.
include(GenVsFilters)
include(SetCompileFlags)

file(GLOB TEST_SOURCES "src/*.cpp")

# Setup the ${PROJECT_NAME} executable with the source files.
add_executable(${PROJECT_NAME} ${TEST_SOURCES})
target_link_libraries(${PROJECT_NAME} TauUtils::TauUtils)

source_group("src" FILES ${TEST_SOURCES})

# Generate the "filters" for VS. This basically just creates an intuitive folder layout.
GenVsFilters(TEST_SOURCES)

target_sources(${PROJECT_NAME} PRIVATE ${TEST_SOURCES})

add_test(NAME ${PROJECT_NAME} COMMAND ${PROJECT_NAME})

# Set the include directory.
target_include_directories(${PROJECT_NAME} PUBLIC include)
# Set the source directory.
target_include_directories(${PROJECT_NAME} PRIVATE src)

# Set C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

SetCompileFlags(${PROJECT_NAME} PUBLIC PRIVATE "OFF")

target_compile_definitions(${PROJECT_NAME} PRIVATE -DTAU_UTILS_IMPORT_SHARED)
