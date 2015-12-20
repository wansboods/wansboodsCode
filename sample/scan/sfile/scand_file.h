#ifndef __SCAND_FILE_H__
#define __SCAND_FILE_H__


//第一位, 消息类型 : 1 -错误类型 2 -
//第二~ 四位消息序列号.
//第五位: 所属类型: 0 - 公共库 1- 文件扫描程序2 - 目录扫描程序 2 -硬盘扫描 程序
typedef enum{
	SCAND_OPFILE_OUT_OF_MEMORY = 11112,  


}SCAND_FILE_MESSAGE;

int get_serialno_file_from_harddisk( char * mountp, char *ghdsn, int length );

#endif //__SCAND_FILE_H__