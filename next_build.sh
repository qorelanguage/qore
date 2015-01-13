#!/bin/sh

#  Permission is hereby granted, free of charge, to any person obtaining a
#  copy of this software and associated documentation files (the "Software"),
#  to deal in the Software without restriction, including without limitation
#  the rights to use, copy, modify, merge, publish, distribute, sublicense,
#  and/or sell copies of the Software, and to permit persons to whom the
#  Software is furnished to do so, subject to the following conditions:
#
#  The above copyright notice and this permission notice shall be included in
#  all copies or substantial portions of the Software.
#
#  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
#  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
#  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
#  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
#  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
#  FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
#  DEALINGS IN THE SOFTWARE.
#
#  Note that the Qore library is released under a choice of three open-source
#  licenses: MIT (as above), LGPL 2+, or GPL 2+; see README-LICENSE for more
#  information.

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
    crev=`cat "$file" 2>/dev/null|cut -b15-`
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

    if [ "$major" -gt 0 ]; then
        qore_ver=`printf %d%02d%02d $major $minor $sub`
    else
        qore_ver=`printf %d%02d $minor $sub`
    fi

    printf "#ifndef _QORE_VERSION_H\n#define _QORE_VERSION_H\n#define QORE_VERSION_MAJOR %s\n#define QORE_VERSION_MINOR %s\n#define QORE_VERSION_SUB %s\n#define QORE_VERSION \"%s.%s.%s\"\n#define QORE_VERSION_CODE %s\n#endif\n" $major $minor $sub $major $minor $sub $qore_ver > "$version_tmp"
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

# see if svn is available
if build="$($(dirname "$0")/getrev.sh)"; then
    make_file ${file}
    ok=1
else
    echo "Need svn revision to create ${file}: "
    cat /tmp/getrev.sh.out
    rm /tmp/getrev.sh.out
    exit 1
fi

if [ $ok -ne 1 ]; then
    if [ -f $file ]; then
        build=`cat $file|cut -b15-`
    else
        echo WARNING! $file not found and svn is not available
        build=0
        make_file $build
    fi
fi

make_version
