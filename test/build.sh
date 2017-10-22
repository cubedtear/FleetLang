#!/usr/bin/env bash

echo $1

$1 input.txt -o output.o -v
if [ $? != 0 ]; then
    echo "Error compiling FleetLang code"
    exit 1
fi
clang++ test.cpp output.o -o test -v
rm output.o
if [ $? != 0 ]; then
    echo "Error linking"
    exit 1
fi

echo "------ Test built"