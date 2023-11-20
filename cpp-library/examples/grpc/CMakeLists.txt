cmake_minimum_required(VERSION 3.0.0)
project(json-schema-reduced-cpp)

find_package(Catch2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_subdirectory(src)

include(CTest)
enable_testing()
add_subdirectory(test)
