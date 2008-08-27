#!/bin/bash

dir=`dirname $0`

file=$dir/include/qore/intern/svn-revision.h

ok=0

if [ "$1" = "-b" ]; then
    show_build=1
else
    show_build=0
fi

make_file() 
{
    crev=`cat $file|cut -b15- 2>/dev/null`
    if [ "$crev" != "$build" ]; then
	printf "#define BUILD %s\n" $build > $1
	echo svn revision changed to $build in $file
    elif [ $show_build -eq 1 ]; then
	echo $build
    fi
}

# see if svnversion is available
which svnversion >/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
    build=`svnversion|sed s/M//|sed s/:.*$//`
    if [ $build != "exported" ]; then
	make_file $file
	ok=1
    fi
fi

if [ $ok -ne 1 ]; then
    if [ ! -f $file ]; then
	build=`cat $file|cut -b15-`
    else
	echo WARNING! $file not found and svnversion is not available
	build=0
	make_file $build
    fi
fi
