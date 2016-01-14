#!/bin/sh

export CPWD=`pwd`
export LIBXML2_PATH=$CPWD/thirdparty/libxml2/library
export BAISC_PATH=$CPWD/thirdparty/basic/library
export MYSQL_PZTH=$CPWD/thirdparty/mysql/library
export PKMYSQL_PZTH=$CPWD/thirdparty/pkmysql/library
export CJSON_PATH=$CPWD/thirdparty/cJSON/library

export LD_LIBRARY_PATH=$LIBXML2_PATH:$BAISC_PATH:$MYSQL_PZTH:$PKMYSQL_PZTH:$CJSON_PATH
