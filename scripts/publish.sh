#!/usr/bin/env bash

set -e

conan install . --build missing -s compiler.cppstd=20 -s build_type=Release -o:a "&:shared=True"
cmake --preset conan-release -DNO_GEN_TEST=0
cmake --build --preset conan-release
conan create . -s build_type=Release -o:a "&:shared=True"
conan export-pkg . -s build_type=Release -o:a "&:shared=True"

conan install . --build missing -s compiler.cppstd=20 -s build_type=Release -o:a "&:shared=False"
cmake --preset conan-release -DNO_GEN_TEST=0
cmake --build --preset conan-release
conan create . -s build_type=Release -o:a "&:shared=False"
conan export-pkg . -s build_type=Release -o:a "&:shared=False"

conan install . --build missing -s compiler.cppstd=20 -s build_type=Debug -o:a "&:shared=True"
cmake --preset conan-debug -DNO_GEN_TEST=0
cmake --build --preset conan-debug
conan create . -s build_type=Debug -o:a "&:shared=True"
conan export-pkg . -s build_type=Debug -o:a "&:shared=True"

conan install . --build missing -s compiler.cppstd=20 -s build_type=Debug -o:a "&:shared=False"
cmake --preset conan-debug -DNO_GEN_TEST=0
cmake --build --preset conan-debug
conan create . -s build_type=Debug -o:a "&:shared=False"
conan export-pkg . -s build_type=Debug -o:a "&:shared=False"
