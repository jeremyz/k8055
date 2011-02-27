#! /bin/bash
[ ! -d build ] && mkdir build
cd build && cmake .. && make && cd .. && ./libk8055/test
