#!/usr/bin/env bash

source ~/env/color/color_print.sh

path="$(cd $(dirname $0) && pwd)"

if [ -d "${path}/build" ]; then
    rm -f ${path}/build/CMakeCache.txt
fi

cmake -B build                            \
      -DCMAKE_BUILD_TYPE=Debug            \
      -DCMAKE_EXPORT_COMPILE_COMMANDS=ON  \
      -S ./                               \
      -GNinja

cmake --build build
