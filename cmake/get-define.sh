#!/bin/sh
grep $1 $2|cut -f3 -d" "
