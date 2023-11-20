set(TEST_EXE_NAME ${PROJECT_NAME}-unit-tests)

find_package( Boost COMPONENTS program_options REQUIRED )

add_executable(
  ${TEST_EXE_NAME}
  catchmain.cpp
  print.cpp
  json.cpp
  accessAttributeByName.cpp
  convert_helper.cpp
  convert.cpp
  program_options.cpp
  type_traits.cpp
  virtual_meta.cpp)

target_link_libraries(${TEST_EXE_NAME} PRIVATE examples meta Catch2::Catch2)
target_link_libraries(${TEST_EXE_NAME} PRIVATE ${Boost_LIBRARIES} )

include(Catch)
catch_discover_tests(${TEST_EXE_NAME})
