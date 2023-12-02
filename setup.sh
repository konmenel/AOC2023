#!/bin/bash

usage() {
    echo "Usage: $0 [daynumber]"
}

is_positive_int() {
    [[ $1 -le 0 ]]  2> /dev/null && return 1 || return 0
}

if [[ $# -ne 1 ]]; then
    echo "Incorrect number of arguments"
    usage
    exit 1
fi

if [[ !($1 =~ ^[0-9]+$) ]] || [[ $1 -le 0 ]]; then
    echo "Argument is not a positive integer"
    usage
    exit 1
fi

daynumber=$1
filename=$(printf "./src/day%02d.cpp" $daynumber)

if test -f $filename; then
    echo "File \"$filename\" already exists"
    exit 1
fi


cp ./template.cpp $filename
# text_to_append=$(printf "day%02d" $daynumber)
# awk -v text="$text_to_append" 'NR==1 { $0 = $0 text } {print}' ./Makefile > tmpfile && mv tmpfile ./Makefile
