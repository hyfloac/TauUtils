file(GLOB SOURCES "src/*.cpp")
file(GLOB_RECURSE HEADERS "include/*.hpp" "include/*.h" "include/*.inl")
file(GLOB HEADERS_BASE "include/*.hpp" "include/*.inl")
file(GLOB HEADERS_ALLOCATOR "include/allocator/*.hpp" "include/allocator/*.inl")
file(GLOB HEADERS_DS "include/ds/*.hpp" "include/ds/*.inl")
file(GLOB HEADERS_JSON "include/json/*.hpp" "include/json/*.inl")

set(TUD_SOURCE_FILES ${SOURCES} ${HEADERS_BASE} ${HEADERS_ALLOCATOR} ${HEADERS_DS} ${HEADERS_JSON})

# Setup the Dynamic and Static libraries with the source files.
add_library(TauUtilsDynamicShared SHARED ${TUD_SOURCE_FILES})
add_library(TauUtilsDynamicStatic STATIC ${TUD_SOURCE_FILES})

foreach(_source IN ITEMS ${HEADERS})
    get_filename_component(_source_path "${_source}" PATH)
    string(REPLACE "${CMAKE_SOURCE_DIR}" "" _group_path "${_source_path}")
    string(REPLACE "/" "\\" _group_path "${_group_path}")
    source_group("include\\${_group_path}" FILES "${_source}")
endforeach()

source_group("src" FILES ${SOURCES})

# Set the include directory.
target_include_directories(TauUtilsDynamicShared PUBLIC include)
target_include_directories(TauUtilsDynamicStatic PUBLIC include)
# Set the source directory.
target_include_directories(TauUtilsDynamicShared PRIVATE src)
target_include_directories(TauUtilsDynamicStatic PRIVATE src)

# Include TauUtilsStatic
target_link_libraries(TauUtilsDynamicShared PUBLIC TauUtilsStatic)
target_link_libraries(TauUtilsDynamicStatic PUBLIC TauUtilsStatic)

# Set C++20
target_compile_features(TauUtilsDynamicShared PUBLIC cxx_std_20)
target_compile_features(TauUtilsDynamicStatic PUBLIC cxx_std_20)

# Enable PIC
set_target_properties(TauUtilsDynamicShared PROPERTIES POSITION_INDEPENDENT_CODE ON)
set_target_properties(TauUtilsDynamicStatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Attempt to enable Link Time Optimization
set_target_properties(TauUtilsDynamicShared PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
set_target_properties(TauUtilsDynamicStatic PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
        # using clang with clang-cl front end

        # Disable RTTI and exceptions
        # target_compile_options(TauUtilsDynamicShared PRIVATE -fno-rtti -fno-exceptions)
        # target_compile_options(TauUtilsDynamicStatic PRIVATE -fno-rtti -fno-exceptions)
        
        target_compile_options(TauUtilsDynamicShared PRIVATE -Wno-unknown-attributes)
        target_compile_options(TauUtilsDynamicStatic PRIVATE -Wno-unknown-attributes)
    elseif(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "GNU")
        # using clang with regular front end

        # Disable RTTI and exceptions
        target_compile_options(TauUtilsDynamicShared PRIVATE -fno-rtti -fno-exceptions)
        target_compile_options(TauUtilsDynamicStatic PRIVATE -fno-rtti -fno-exceptions)
    endif()
endif()

target_compile_definitions(TauUtilsDynamicShared PRIVATE -DTAU_UTILS_BUILD_SHARED)
target_compile_definitions(TauUtilsDynamicStatic PRIVATE -DTAU_UTILS_BUILD_STATIC)

if(CMAKE_CXX_COMPILER_FRONTEND_VARIANT STREQUAL "MSVC")
    # Disable exceptions and ignore some CRT warnings
    target_compile_definitions(TauUtilsDynamicShared PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=1)
    target_compile_definitions(TauUtilsDynamicStatic PRIVATE -D_CRT_SECURE_NO_WARNINGS -D_HAS_EXCEPTIONS=1)

    set_target_properties(TauUtilsDynamicShared PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
    set_target_properties(TauUtilsDynamicStatic PROPERTIES MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()
