cmake_minimum_required(VERSION 3.0.0)
project(json-schema-reduced-cpp)

find_package(nlohmann_json REQUIRED)
find_package(Catch2 REQUIRED)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(CTest)
enable_testing()

add_compile_options(-fconcepts-diagnostics-depth=10)

add_subdirectory(src)
add_subdirectory(examples)

add_library(examples INTERFACE)
target_include_directories(examples INTERFACE src-gen)

add_subdirectory(test)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
