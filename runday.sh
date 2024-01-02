#!/bin/bash

set -e

usage() {
    echo "Usage:"
    echo "$0 <daynumber> [release(default)|debug]"
}

if [[ $# -lt 1 ]]; then
    echo "Incorrect number of arguments"
    usage
    exit 1
fi

basedir="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd)"
daynumber=$1
mode=$2

if [[ !($daynumber =~ ^[0-9]+$) ]] || [[ $daynumber -le 0 ]]; then
    echo "Daynumber is not a positive integer"
    usage
    exit 1
fi
daynumber=$(printf "%02d" $daynumber)

debug=false
includes="-I$basedir/include"
gcc_options="-Wall -Wpedantic -std=c++23 $includes"
datadir=data
if [[ "debug" == $mode ]]; then
    debug=true
    gcc_options="$gcc_options -DDEBUG -O0 -g"
    datadir=data/examples
elif [[ "release" == $mode || "" == $mode ]]; then
    debug=false
    gcc_options="$gcc_options -Ofast -ffast-math"
    datadir=data
else
    echo "Unknown mode \"$mode\""
    usage
    exit 1
fi

if [ ! -d "bin" ]; then
    mkdir bin
fi

set -x
g++ ./src/day$daynumber.cpp -o ./bin/day$daynumber $gcc_options
./bin/day$daynumber $datadir/$daynumber.txt
