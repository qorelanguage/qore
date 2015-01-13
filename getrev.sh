#!/bin/sh

if out=$(svn info "$1" 2> /dev/null) || out=$(git svn info "$1" 2> /dev/null); then
    build=$(echo "${out}"|grep "^Revision:"|cut -f2 -d' ')
    echo "${build}"
    exit 0
else
    echo "No version control found."
    exit 1
fi
