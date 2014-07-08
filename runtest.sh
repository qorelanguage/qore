#!/bin/sh

test=$1
if [ \! -f $test -a -f examples/$test ]; then
    test=examples/$test
fi

./qore $test -v
