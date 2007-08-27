#!/bin/sh

sed "s/ //g" t| cut -f1 -d=| cut -f1 -d,| while read a; do printf "   qt->addConstant(%-27s new QoreNode((int64)Qt::%s));\n" \"$a\", $a; done
