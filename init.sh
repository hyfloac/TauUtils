#!/usr/bin/env bash

conan install . --output-folder=build --build=missing --settings=build_type=Debug
conan install . --output-folder=build --build=missing --settings=build_type=Release
cmake --preset conan-debug -DNO_GEN_TEST=0

