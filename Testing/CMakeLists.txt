file(GLOB SOURCES "src/*.cpp")

# Setup the testing executable with the source files.
add_executable(Testing ${SOURCES})

# Set the include directory.
target_include_directories(Testing PUBLIC include)
# Set the source directory.
target_include_directories(Testing PRIVATE src)

# Include TauUtilsStatic
target_link_libraries(Testing PUBLIC TauUtilsStatic)
# Include TauUtilsDynamic
#target_link_libraries(Testing PUBLIC TauUtilsDynamicStatic)
# Include TauUtilsDynamic
target_link_libraries(Testing PUBLIC TauUtilsDynamicShared)

# Set C++20
target_compile_features(Testing PUBLIC cxx_std_20)

# Enable PIC
set_target_properties(Testing PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Attempt to enable Link Time Optimization
set_target_properties(Testing PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)

add_custom_command(TARGET Testing POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:TauUtilsDynamicShared> $<TARGET_FILE_DIR:Testing>)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # using clang with clang-cl front end

        # Disable RTTI and exceptions
        # target_compile_options(Testing PRIVATE -fno-rtti -fno-exceptions)
        target_compile_options(Testing PRIVATE -Wno-unknown-attributes)
    elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        # using clang with regular front end

        # Disable RTTI and exceptions
        target_compile_options(Testing PRIVATE -fno-rtti -fno-exceptions)
        target_compile_options(Testing PRIVATE -Wno-unknown-attributes)
    endif()
endif()

target_compile_definitions(Testing PRIVATE -DTAU_UTILS_IMPORT_SHARED)

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # Disable exceptions and ignore some CRT warnings
    target_compile_definitions(Testing PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=1)
    
    set_target_properties(Testing PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    # set_target_properties(Testing PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
