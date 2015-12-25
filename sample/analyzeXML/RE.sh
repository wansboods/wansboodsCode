#!/bin/sh

export CPWD=`pwd`
export CXML_PATH=$CPWD/build/library
export XML_PATH=$CPWD/lib/thirdparty/lib
export LD_LIBRARY_PATH=$XML_PATH:$CXML_PATH:$LD_LIBRARY_PATH

