#!/bin/bash
P=$(pwd)
cd optimize
virtualenv -p "$(command -v python3)" ./venv
./venv/bin/python setup.py develop
./venv/bin/python __main__.py
cd $P
