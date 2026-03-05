#!/bin/sh

if [ $# -gt 0 ]; then
	TARGET=$@
fi

export NINJA_STATUS="[%f/%t] "
cd ./out/build/dev-linux-release && ninja -j 8 -v $TARGET
git rev-parse HEAD > ./version.txt