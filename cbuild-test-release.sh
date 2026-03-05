#!/bin/sh

mkdir -p ./out/build/test-linux-release
cd ./out/build/test-linux-release && cmake --preset test-linux-release ../../../
