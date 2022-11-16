add_library(TauUtilsStatic STATIC src/IntMath.ll)

target_include_directories(TauUtilsStatic PUBLIC include)
target_include_directories(TauUtilsStatic PRIVATE src)

set_target_properties(TauUtilsStatic PROPERTIES LINKER_LANGUAGE CXX)
