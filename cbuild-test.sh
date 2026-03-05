#!/bin/sh

mkdir -p ./out/build/test-linux-debug
cd ./out/build/test-linux-debug && cmake --preset test-linux-debug ../../../
