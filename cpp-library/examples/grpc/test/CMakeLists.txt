set(TEST_EXE_NAME grpc-unit-tests)

find_package( Boost COMPONENTS program_options REQUIRED )

add_executable(
  ${TEST_EXE_NAME}
  catchmain.cpp
  grpc.cpp)

target_link_libraries(${TEST_EXE_NAME} PRIVATE grpc_examples examples meta Catch2::Catch2)
target_link_libraries(${TEST_EXE_NAME} PRIVATE ${Boost_LIBRARIES} )

include(Catch)
catch_discover_tests(${TEST_EXE_NAME})
