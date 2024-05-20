@echo off
conan create . -s build_type=Release -o shared=True
conan create . -s build_type=Release -o shared=False
conan create . -s build_type=Debug -o shared=True
conan create . -s build_type=Debug -o shared=False
