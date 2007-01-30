#!/bin/sh

rm -f config.cache acconfig.h aclocal.m4
cat m4/*.m4 > acinclude.m4
aclocal -I m4
autoconf
#acconfig
autoheader
automake -a

