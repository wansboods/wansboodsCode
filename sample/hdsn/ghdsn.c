#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <fcntl.h>
#include <errno.h>
// 获取硬盘序列号，成功返回0， 失败返回-1
// szDevName为取得的序列号，空间由主程序预先分配好，并将空间长度用nLimit参数传递给get_hd_sn()函数
int get_hd_sn(const char* szDevName, char* szSN, size_t nLimit)
{
    struct hd_driveid id;
    int  nRtn = -1;
int  fd = open(szDevName, O_RDONLY|O_NONBLOCK); 
    while(1)
    {
        if (fd < 0)
        {
            perror(szDevName);
            break;
        }
//这句话是关键
        if(!ioctl(fd, HDIO_GET_IDENTITY, &id))
        {
             strncpy(szSN, id.serial_no, nLimit);
             //printf("Model Number=%s\n",id.model);
             nRtn = 0;
        }
        break;
    }
    return nRtn;
}

int main( ){

    char buf[128] ={'\0'};
    get_hd_sn( "/dev/sda", buf, sizeof( buf) );
    printf( "buf:%s\n", buf );

    return 0;
}
