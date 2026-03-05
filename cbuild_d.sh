#!/bin/sh

mkdir -p ./out/build/dev-linux-debug
cd ./out/build/dev-linux-debug && cmake -DCMAKE_BUILD_TYPE=Debug ../../../
