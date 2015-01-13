#!/bin/sh

#if out=$(svn info "$1" 2> /dev/null) || out=$(git svn info "$1" 2> /dev/null); then
if out=$(svn info "$1" 2> /tmp/getrev.sh.out) || out=$(git svn info "$1" 2> /tmp/getrev.sh.out); then
    build=$(echo "${out}"|grep "^Revision:"|cut -f2 -d' ')
    echo "${build}"
    rm /tmp/getrev.sh.out
    exit 0
else
    #echo "No version control found."
    printf "No version control found: "
    cat /tmp/getrev.sh.out
    rm /tmp/getrev.sh.out
    exit 1
fi
