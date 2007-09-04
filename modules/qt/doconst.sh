#!/bin/sh

if [ -n "$1" ]; then
    ns=$1
else
    ns=qt
fi

sed "s/ //g" t| cut -f1 -d=| cut -f1 -d,| while read a; do printf "   $ns->addConstant(%-27s new QoreNode((int64)Qt::%s));\n" \"$a\", $a; done
