#!/bin/bash

rm -rf build
mkdir build 
qmake -o build/Makefile
cd build && make 
