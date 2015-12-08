#!/bin/bash
# shell脚本，创建 mysql数据库、用户名、密码。 
# 同时添加用户权限。
#-----------------------------
DBNAME=charging
USERNAME=root
PASSWORD=root
PORT=3306
HOSTNAME=127.0.0.1

create_user_sql="create user charging IDENTIFIED by 'bevix2015'"
mysql -u${USERNAME} -p${PASSWORD} -e "${create_user_sql}"

create_db_sql="create database IF NOT EXISTS ${DBNAME}"
#mysql -h${HOSTNAME} -P${PORT} -u${USERNAME} -p${PASSWORD} -e "${create_db_sql}"
mysql -u${USERNAME} -p${PASSWORD} -e "${create_db_sql}"


MYSQL=`which mysql`
mysql -u${USERNAME} -p${PASSWORD} << EOF
use mysql;
GRANT ALL PRIVILEGES ON charging.* TO charging@'%' IDENTIFIED BY 'bevix2015' WITH GRANT OPTION;
EOF

echo "创建数据库已完成,准备重启MYSQL服务器"
/etc/init.d/mysqld restart
echo "重启完成"
echo "完成 MYSQL 安装!"
