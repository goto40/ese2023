#!/bin/bash
set -e

python3 -m venv venv
. ./venv/bin/activate
pip install -r requirements.txt
npx json-schema-reduced-py-codegen model/*
pytest $*
