cmake_minimum_required(VERSION 3.0.0)
project(dds_examples)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find requirements
find_package(Catch2 REQUIRED)
if(NOT fastcdr_FOUND)
    find_package(fastcdr REQUIRED)
endif()

if(NOT fastrtps_FOUND)
    find_package(fastrtps REQUIRED)
endif()

include(CTest)
enable_testing()

file(GLOB_RECURSE SRC src/*.cpp src/*.cxx)
file(GLOB EXAMPLES examples/*.cpp)

add_library(${PROJECT_NAME} ${SRC} ${version_file})
target_include_directories(${PROJECT_NAME} PUBLIC src)
target_link_libraries(${PROJECT_NAME} fastrtps fastcdr)

set(EXE_NAME "${PROJECT_NAME}.exe")
foreach(CPPFILE ${EXAMPLES})
    get_filename_component(EXE_NAME ${CPPFILE} NAME_WLE)
    add_executable(${EXE_NAME} ${CPPFILE})
    target_include_directories(${PROJECT_NAME} PRIVATE src)
    target_link_libraries(${EXE_NAME} ${PROJECT_NAME})
endforeach()

add_subdirectory(test)
