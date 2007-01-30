#!/bin/bash

build=`cat .build`
build=$(( $build + 1 ))
echo $build > .build
echo $build

