#!/bin/sh

if [ -z "$1" ]; then
    echo missing file name
else
    svn info "$1" | grep Revision | sed "s/Revision: //"
fi
