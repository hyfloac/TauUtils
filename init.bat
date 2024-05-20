@echo off
conan install . --output-folder=build --build=missing --settings=build_type=Debug
conan install . --output-folder=build --build=missing --settings=build_type=Release
cmake --preset conan-default -DNO_GEN_TEST=0
