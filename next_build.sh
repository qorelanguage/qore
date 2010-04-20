#!/bin/sh

dir=`dirname $0`

qore_inc="$dir/include/qore"
file="$qore_inc/intern/svn-revision.h"
version_file="$qore_inc/qore-version.h"
version_tmp="$qore_inc/qore-version.h.tmp"

ok=0

if [ "$1" = "-b" ]; then
    show_build=1
else
    show_build=0
fi

make_file() {
    crev=`cat "$file"|cut -b15- 2>/dev/null`
    if [ "$crev" != "$build" ]; then
	printf "#define BUILD %s\n" $build > $1
	echo svn revision changed to $build in $file
    elif [ $show_build -eq 1 ]; then
	echo $build
    fi
}

make_version() {
    major=`grep define.VERSION_MAJOR "$qore_inc/intern/unix-config.h"|cut -f3 -d\ `
    minor=`grep define.VERSION_MINOR "$qore_inc/intern/unix-config.h"|cut -f3 -d\ `
    sub=`grep define.VERSION_SUB "$qore_inc/intern/unix-config.h"|cut -f3 -d\ `

    printf "#ifndef _QORE_VERSION_H\n#define _QORE_VERSION_H\n#define QORE_VERSION_MAJOR %s\n#define QORE_VERSION_MINOR %s\n#define QORE_VERSION_SUB %s\n#define QORE_VERSION \"%s.%s.%s\"\n#define QORE_VERSION_CODE %02d%02d%02d\n#endif\n" $major $minor $sub $major $minor $sub $major $minor $sub > "$version_tmp"
    create=yes
    if [ -f "$version_file" ]; then
	diff "$version_tmp" "$version_file" >/dev/null 2>/dev/null
	if [ $? -eq 0 ]; then
	    create=no
	fi
    fi

    if [ "$create" = "yes" ]; then
	mv "$version_tmp" "$version_file"
	printf "created $version_file for qore %s.%s.%s\n" $major $minor $sub
    else
	rm -f "$version_tmp"
    fi
}

# see if svnversion is available
which svnversion >/dev/null 2>/dev/null
if [ $? -eq 0 ]; then
    build=`svnversion|sed s/M//|sed s/:.*$//`
    if [ "$build" != "exported" ]; then
	make_file $file
	ok=1
    fi
fi

if [ $ok -ne 1 ]; then
    if [ -f $file ]; then
	build=`cat $file|cut -b15-`
    else
	echo WARNING! $file not found and svnversion is not available
	build=0
	make_file $build
    fi
fi

make_version
