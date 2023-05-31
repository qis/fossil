cmake_minimum_required(VERSION 3.26 FATAL_ERROR)
project(project_name VERSION 0.1.0 LANGUAGES CXX)

file(GLOB headers src/*.hpp)
file(GLOB sources src/*.cpp)

add_executable(main ${headers} ${sources})
target_compile_features(main PRIVATE cxx_std_20)
