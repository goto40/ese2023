#!/bin/bash
set -e

cd .. && tar cvzf $1ese-$(date +"%Y-%m-%d-%H-%M").tgz --exclude "./ese/node_modules" --exclude "./ese/packages/*node_modules" --exclude "./ese/packages/*/out" --exclude "./ese/cpp-library/build" --exclude "ese/cpp-library/.cache" --exclude "**/.pytest_cache" --exclude "**/__pycache__" --exclude "ese/python-library/venv" ./ese
