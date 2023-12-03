#!/bin/bash
set -e

# generate code from our main code generator (src-gen, not checked in; see reference folders for demos)
npx json-schema-reduced-cpp-codegen model/*.json

# generate code for idl files (under version control)
fastddsgen -replace examples/fastdds/src/dds_examples/*.idl
for idl in ./examples/fastdds/src/dds_examples/*.idl; do
  echo "generating ${idl%.idl}.meta.h"
  idl2meta $idl > ${idl%.idl}.meta.h
done

# generate code for proto files (under version control)
pushd examples/grpc/model
chmod a+x ../../../../protoc-meta-plugin.py
protoc --plugin=protoc-gen-meta-plugin=../../../../protoc-meta-plugin.py --meta-plugin_out=../src/ --cpp_out=../src/ -I=./ *.proto
popd

cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -B build
cmake --build build
ctest --test-dir build --output-on-failure
