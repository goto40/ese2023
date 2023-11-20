file(GLOB EXAMPLES ./*.cpp)

find_package( Boost COMPONENTS program_options REQUIRED )

foreach(CPPFILE ${EXAMPLES})
    get_filename_component(EXE_NAME ${CPPFILE} NAME_WLE)
    add_executable(${EXE_NAME} ${CPPFILE})
    target_include_directories(${EXE_NAME} PRIVATE ../src-gen)
    target_link_libraries(${EXE_NAME} PRIVATE meta)
endforeach()

target_include_directories( commandline PRIVATE ${Boost_INCLUDE_DIR})
target_link_libraries( commandline PRIVATE ${Boost_LIBRARIES} )

add_subdirectory(mqtt)
add_subdirectory(fastdds)
add_subdirectory(grpc)
