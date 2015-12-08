#!/bin/sh
########################################################
# 在centos服务器安装MYSQL数据库
# @version: 0.1
# @date: 2015-11-13
# @author:
########################################################

MYCNF_PATH=my.cnf
MYSH_PATH=my.sh

if [ ! -f $MYCNF_PATH ]
then
    echo "当前目录下未找到my.cnf文件! 终止安装MYSQL程序.."
    exit
fi

if [ ! -f $MYSH_PATH ]
then
    echo "当前目录下未找到my.sh脚本! 终止安装MYSQL程序.."
fi

echo "正在导入 MYSQL RPM包.."
yum install http://dev.mysql.com/get/mysql-community-release-el6-5.noarch.rpm
echo "......导入成功!"

echo "正在安装 MYSQL 安装包.."
yum -qy install mysql mysql-devel mysql-server mysql-utilities
echo "......安装包 安装完毕!"

echo "启动 MYSQL 服务"
/etc/init.d/mysqld start

echo "设置 MYSQL ROOT密码"
mysqladmin -u root password 'root'

echo "正在导入配置项"
cp $MYCNF_PATH /etc/my.cnf 
echo "配置项导入成功!"

echo "设置自启动 MYSQL 服务"
chkconfig --level 35 mysqld on

echo "重启 MYSQL 服务"
/etc/init.d/mysqld restart
echo "MYSQL 服务已启动!"



source ./my.sh
