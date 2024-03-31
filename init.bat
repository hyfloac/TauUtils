@echo off
conan install . --output-folder=build --build=missing --settings=build_type=Debug
cmake --preset conan-default -DNO_GEN_TEST=0
