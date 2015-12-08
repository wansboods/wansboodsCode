#!/bin/sh
    ps -e -o pid -o etime -o args|grep oracle|grep LOCAL=NO >> /tmp/tmpfile
#ps -e -o pid -o etime -o args|grep oracle|grep LOCAL=NO|grep -|awk '{print $2}'|awk -F: '{print $1}'
cat /tmp/tmpfile | while read LINE
do
TIME=`echo $LINE | awk '{print $2}'`
TIME=`echo $TIME | awk -F '{print $1}'`
#if [ $TIME -gt 3 ]
#then
echo $LINE | awk '{print $1}'| xargs -t -n1 kill -9
#fi 
done
rm -f /tmp/tmpfile
