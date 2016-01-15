#!/bin/sh

export CPWD=`pwd`
export LIBXML2_PATH=$CPWD/thirdparty/libxml2/lib
export ZLOG_PATH=$CPWD/thirdparty/zlog
#export BAISC_PATH=$CPWD/thirdparty/basic/library
#export MYSQL_PZTH=$CPWD/thirdparty/mysql/library
#export PKMYSQL_PZTH=$CPWD/thirdparty/pkmysql/library
export TRLV_PZTH=$CPWD/thirdparty/trlv/
export CJSON_PATH=$CPWD/thirdparty/cJSON/library
export ANALYZEXML_PATH=$CPWD/thirdparty/analyzeXML/library
export BASIC_PATH=$CPWD/thirdparty/basic/library


export LD_LIBRARY_PATH=$ANALYZEXML_PATH:$ZLOG_PATH:$LIBXML2_PATH:$BAISC_PATH:$MYSQL_PZTH:$PKMYSQL_PZTH:$CJSON_PATH:$BASIC_PATH
