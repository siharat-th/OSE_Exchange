#!/bin/sh

mkdir -p ./out/build/dev-linux-release
cd ./out/build/dev-linux-release && cmake --preset dev-linux-release ../../../
