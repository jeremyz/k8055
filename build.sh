#! /bin/bash

VERBOSE=${VERBOSE:-0}
CMAKE_OPTIONS=""
MAKE_OPTIONS=""
BUILD_DIR=./build

[ -f cmake_opts ] && source cmake_opts

function cmake_init() {
    echo -e " * cmake init\n"
    if [ ! -d $BUILD_DIR ]; then
        mkdir $BUILD_DIR || exit 1
    fi
    cd $BUILD_DIR || exit 1
    if [ ! -e CMakeCache.txt ]; then
        cmake ${CMAKE_OPTIONS} .. || exit 1
    fi
    cd .. || exit 1
}

function cmake_clean() {
    echo -e " * clean cmake files\n" && rm $BUILD_DIR/CMakeCache.txt 2>/dev/null
}

function cmake_rm() {
    echo -e " * rm cmake files\n" && rm -fr $BUILD_DIR 2>/dev/null
}

function cmake_make() {
    cmake_init
    echo -e " * cmake make\n" && cd $BUILD_DIR || exit 1
    if [ $VERBOSE -eq 1 ]; then
        VERBOSE=1 make $MAKE_OPTIONS || exit 1
    else
        make $MAKE_OPTIONS || exit 1
    fi
    cd ..
}

function cmake_install() {
    cmake_init
    echo -e " * install" && cd $BUILD_DIR && sudo make install && cd .. || exit 1
}

function cmake_tests() {
    cmake_init
    echo -e " * execute tests\n" && time ./libk8055/test
}

if [ $# -eq 0 ]; then
    echo "usage $0 [cmds list]"
    echo "cmds may be"
    echo "   r[m]      => all built, temp and cache files"
    echo "   c[lean]   => remove cache files"
    echo "   m[ake]    => launch the build process"
    echo "   t[ests]   => execute tests"
    echo "   i[nstall] => install libk8055 k8055, and k8055gui"
    echo "ex: $0 r m t i"
    exit 1
fi

for arg in $@; do
    case $arg in
        c|clean)
            cmd="cmake_clean";;
        r|rm)
            cmd="cmake_rm";;
        m|make)
            cmd="cmake_make";;
        t|test)
            cmd="cmake_tests";;
        i|install)
            cmd="cmake_install";;
        *)
         echo "unknown command ${arg}" && exit 1
     esac
     $cmd
done
