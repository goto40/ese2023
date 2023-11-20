#!/bin/bash
set -e

cd cpp-library
./build-and-test.sh
cd ..

cd python-library
./run_tests.sh
cd ..
