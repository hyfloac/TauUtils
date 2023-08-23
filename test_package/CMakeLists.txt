cmake_minimum_required(VERSION 3.23)
project(TauUtilsTest LANGUAGES CXX C)

include(CheckCCompilerFlag)

find_package(tauutils CONFIG REQUIRED)

file(GLOB SOURCES "src/*.cpp")

# Setup the ${PROJECT_NAME} executable with the source files.
add_executable(${PROJECT_NAME} ${SOURCES})
target_link_libraries(${PROJECT_NAME} tauutils::TauUtilsDynamicShared)

# Set the include directory.
target_include_directories(${PROJECT_NAME} PUBLIC include)
# Set the source directory.
target_include_directories(${PROJECT_NAME} PRIVATE src)

# # Include TauUtilsStatic
# target_link_libraries(${PROJECT_NAME} PUBLIC TauUtilsStatic)
# Include TauUtilsDynamic
#target_link_libraries(${PROJECT_NAME} PUBLIC TauUtilsDynamicStatic)
# Include TauUtilsDynamic
# target_link_libraries(${PROJECT_NAME} PUBLIC TauUtilsDynamicShared)

# Set C++20
target_compile_features(${PROJECT_NAME} PUBLIC cxx_std_20)

# Enable PIC
# set_target_properties(${PROJECT_NAME} PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Attempt to enable Link Time Optimization
# set_target_properties(${PROJECT_NAME} PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)

# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:TauUtilsDynamicShared> $<TARGET_FILE_DIR:${PROJECT_NAME}>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # using clang with clang-cl front end

        # Disable RTTI and exceptions
        # target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti -fno-exceptions)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unknown-attributes)
    elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        # using clang with regular front end

        # Disable RTTI and exceptions
        target_compile_options(${PROJECT_NAME} PRIVATE -fno-rtti -fno-exceptions)
        target_compile_options(${PROJECT_NAME} PRIVATE -Wno-unknown-attributes)
    endif()
endif()

target_compile_definitions(${PROJECT_NAME} PRIVATE -DTAU_UTILS_IMPORT_SHARED)

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # Disable exceptions and ignore some CRT warnings
    target_compile_definitions(${PROJECT_NAME} PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=1)
    
    set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    # set_target_properties(${PROJECT_NAME} PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

check_c_compiler_flag(/wd5030 HAS_UNRECOGNIZED_ATTRIBUTES_WARNING)
check_c_compiler_flag(/wd4251 HAS_DLL_INTERFACE_WARNING)
check_c_compiler_flag(/wd4068 HAS_CLANG_PRAGMA_WARNING)

if(HAS_UNRECOGNIZED_ATTRIBUTES_WARNING)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd5030)
endif()

if(HAS_DLL_INTERFACE_WARNING)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4251)
endif()

if(HAS_CLANG_PRAGMA_WARNING)
    target_compile_options(${PROJECT_NAME} PRIVATE /wd4068)
endif()
