# for protobuf code generation see /cpp-library/build-and-test.sh
# add_custom_command(
#   OUTPUT
#     ${CMAKE_CURRENT_LIST_DIR}/Simple.pb.h
#     ${CMAKE_CURRENT_LIST_DIR}/Simple.pb.cc
#    ${CMAKE_CURRENT_LIST_DIR}/HelloWorld.pb.h
#    ${CMAKE_CURRENT_LIST_DIR}/HelloWorld.pb.cc
#   COMMAND protoc -I../model --cpp_out=./ ../model/*.proto
#   DEPENDS ../model/Simple.proto ../model/HelloWorld.proto
#   WORKING_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}
# )

add_library(grpc_examples)
target_include_directories(grpc_examples PUBLIC ../src)
target_link_libraries(grpc_examples PRIVATE protobuf meta)
target_sources(grpc_examples PRIVATE Simple.pb.cc HelloWorld.pb.cc)
