#!/usr/bin/env bash

conan install . --build missing -s compiler.cppstd=20 -s build_type=Debug
conan install . --build missing -s compiler.cppstd=20 -s build_type=Release
cmake --preset conan-debug -DNO_GEN_TEST=0
cmake --preset conan-release -DNO_GEN_TEST=0
