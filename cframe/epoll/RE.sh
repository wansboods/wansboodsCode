#!/bin/sh

export CPWD=`pwd`
export LIBXML2_PATH=$CPWD/thirdparty/libxml2/library
export BAISC_PATH=$CPWD/thirdparty/basic/library
export LD_LIBRARY_PATH=$LIBXML2_PATH:$BAISC_PATH
