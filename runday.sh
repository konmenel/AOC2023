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

daynumber=$1
mode=$2

if [[ !($daynumber =~ ^[0-9]+$) ]] || [[ $daynumber -le 0 ]]; then
    echo "Daynumber is not a positive integer"
    usage
    exit 1
fi
daynumber=$(printf "%02d" $daynumber)

debug=false
gcc_flacs=""
datadir=data
if [[ "debug" == $mode ]]; then
    debug=true
    gcc_flacs="-DDEBUG -O0 -g"
    datadir=data/examples
elif [[ "release" == $mode || "" == $mode ]]; then
    debug=false
    gcc_flacs="-O3"
    datadir=data
else
    echo "Unknown mode \"$mode\""
    usage
    exit 1
fi

set -x
g++ ./src/day$daynumber.cpp -o ./bin/day$daynumber $gcc_flacs
./bin/day$daynumber $datadir/$daynumber.txt
