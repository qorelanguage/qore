#!/bin/sh

rel=`qore -XVersionString|sed s/\"//g`

zip -r qore-$rel-Windows.zip bin/ include/ lib/ README*
