#!/bin/sh

if [ $# -gt 0 ]; then
	TARGET=$@
fi

cd ./out/build/dev-linux-debug && make -j8 VERBOSE=1 $TARGET
git rev-parse HEAD > ./out/build/dev-linux-debug/version.txt