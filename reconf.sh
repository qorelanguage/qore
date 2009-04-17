#!/bin/sh
# see if we need libtoolize or glibtoolize
which libtoolize >/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
   lcmd=libtoolize
else
    which glibtoolize >/dev/null 2>/dev/null
    if [ $? -eq 0 ]; then
	lcmd=glibtoolize
    fi
fi
if [ -z "$lcmd" ]; then
    echo ERROR: please install libtoolize or glibtoolize before running this script
fi
set -x
rm -f config.cache acconfig.h aclocal.m4 config.guess config.sub ltmain.sh
$lcmd
cat m4/*.m4 > acinclude.m4
aclocal -I m4
autoconf
#acconfig
autoheader
automake -a

