#!/bin/sh

if [ -n "$1" ]; then
    ns=$1
else
    ns=qt
fi

if [ -n "$2" ]; then
    cns=$2
else
    cns=Qt
fi

sed "s/ //g" t| cut -f1 -d=| cut -f1 -d,| grep -v ^$ | while read a; do printf "   $ns->addConstant(%-27s new QoreBigIntNode($cns::%s));\n" \"$a\", $a; done
