#!/usr/bin/env bash

set -e

for release in Release Debug
do
  for shared in True False
  do
    rm -r ./build/
    conan install . --build missing -s compiler.cppstd=20 -s build_type=$release -o:a "&:shared=$shared"
    cmake --preset conan-${release,,} -DNO_GEN_TEST=0
    cmake --build --preset conan-${release,,}
    conan create . -s build_type=$release -o:a "&:shared=$shared"
    conan export-pkg . -s build_type=$release -o:a "&:shared=$shared"
  done
done
