roc_name_01="xx"  #进程名
proc_num()                     #查询进程数量
{
    num=`ps -ef | grep $proc_name_01 | grep -v grep | wc -l`
    return $num
}

proc_num
number=$?                       #获取进程数量
if [ $number -eq 0 ]            #如果进程数量为0
then                            #重新启动服务器，或者扩展其它内容。
        echo "-------------------------------------------------" >> /tmp/monitor.log
        date +"%T %a %D : 检测出进程hdplayermanage已经不在!"     >>  /tmp/monitor.log
        cd /data/d2dcopy/running/bin/; ./hdplayermanage &
        date +" %T %a %D : 程序hdplayermanage进程已经被拉起!"    >> /tmp/monitor.log
        echo "-------------------------------------------------" >> /tmp/monitor.log
        echo "Done"
fi
~  
