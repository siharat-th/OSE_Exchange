#!/bin/sh

if [ $# -gt 0 ]; then
	TARGET=$@
fi

export NINJA_STATUS="[%f/%t] "
cd ./out/build/test-linux-debug && ninja -j 16 -v $TARGET
git rev-parse HEAD > ./version.txt
