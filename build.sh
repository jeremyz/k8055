#! /bin/bash
[ ! -d build ] && mkdir build
cd build && cmake -DWANT_DEBUG=1 .. && make && cd .. && time ./libk8055/test
