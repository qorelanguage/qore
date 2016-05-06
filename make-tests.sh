#!/bin/sh

echo -n 'qoretests =' > qoretests.mk
find examples/test/ -type f -name \*.qtest -print | sed 's/^/  /;$q;s/$/ \\/' >> qoretests.mk
