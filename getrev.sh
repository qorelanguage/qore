#!/usr/bin/env bash

# very fast check without calling exec()
if [ -n "$BASH_VERSION" -o "$KSH_VERSION" ]; then
    if [[ "$1" = *dox* ]]; then 
	echo "ignoring dox file $1"
	exit 1
    fi
fi

if [ -d `dirname $0`/.git ]; then
    git rev-parse HEAD
    exit 0
else
    echo "No version control found."
    exit 1
fi
