cmake_minimum_required(VERSION 3.23)

include(CheckCCompilerFlag)

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

target_sources(TauUtilsDynamicShared PUBLIC ${SOURCES})
target_sources(TauUtilsDynamicStatic PUBLIC ${SOURCES})

target_sources(TauUtilsDynamicShared PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${HEADERS})
target_sources(TauUtilsDynamicStatic PUBLIC FILE_SET HEADERS BASE_DIRS include FILES ${HEADERS})


if(NOT ${NO_GEN_TEST})
    add_custom_command(TARGET TauUtilsDynamicShared POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:TauUtilsDynamicShared> $<TARGET_FILE_DIR:TauUtilsTest>)
endif()

# Set the include directory.
target_include_directories(TauUtilsDynamicShared PUBLIC include)
target_include_directories(TauUtilsDynamicStatic PUBLIC include)
# Set the source directory.
target_include_directories(TauUtilsDynamicShared PRIVATE src)
target_include_directories(TauUtilsDynamicStatic PRIVATE src)

# # Include TauUtilsStatic
# target_link_libraries(TauUtilsDynamicShared PUBLIC TauUtilsStatic)
# target_link_libraries(TauUtilsDynamicStatic PUBLIC TauUtilsStatic)

# Set C++20
target_compile_features(TauUtilsDynamicShared PUBLIC cxx_std_20)
target_compile_features(TauUtilsDynamicStatic PUBLIC cxx_std_20)


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

        # Enable PIC
        set_target_properties(TauUtilsDynamicShared PROPERTIES POSITION_INDEPENDENT_CODE ON)
        set_target_properties(TauUtilsDynamicStatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

        # Attempt to enable Link Time Optimization
        set_target_properties(TauUtilsDynamicShared PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
        set_target_properties(TauUtilsDynamicStatic PROPERTIES INTERPROCEDURAL_OPTIMIZATION ON)
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
    
    target_compile_options(TauUtilsDynamicShared PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/Zi>")
    target_link_options(TauUtilsDynamicShared PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/DEBUG>")
    target_link_options(TauUtilsDynamicShared PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF>")
    target_link_options(TauUtilsDynamicShared PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>")
    
    target_compile_options(TauUtilsDynamicStatic PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/Zi>")
    target_link_options(TauUtilsDynamicStatic PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/DEBUG>")
    target_link_options(TauUtilsDynamicStatic PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/OPT:REF>")
    target_link_options(TauUtilsDynamicStatic PRIVATE "$<$<NOT:$<CONFIG:Debug>>:/OPT:ICF>")
endif()

check_c_compiler_flag(/wd5030 HAS_UNRECOGNIZED_ATTRIBUTES_WARNING)
check_c_compiler_flag(/wd4251 HAS_DLL_INTERFACE_WARNING)

if(HAS_UNRECOGNIZED_ATTRIBUTES_WARNING)
    target_compile_options(TauUtilsDynamicShared PRIVATE /wd5030)
endif()

if(HAS_DLL_INTERFACE_WARNING)
    target_compile_options(TauUtilsDynamicShared PRIVATE /wd4251)
endif()

install(
    TARGETS TauUtilsDynamicShared TauUtilsDynamicStatic
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FILE_SET HEADERS
)
