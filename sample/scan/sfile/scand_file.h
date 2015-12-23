#ifndef __SCAND_FILE_H__
#define __SCAND_FILE_H__

#include "scand_harddisk.h"

//第一位, 消息类型 : 1 -错误类型 2 -
//第二~ 四位消息序列号.
//第五位: 所属类型: 0 - 公共库 1- 文件扫描程序2 - 目录扫描程序 2 -硬盘扫描 程序
typedef enum{
	SCAND_OPFILE_OUT_OF_MEMORY = 11112,  


}SCAND_FILE_MESSAGE;

typedef enum{
	SCAND_OTHER_FILE  = 0,
    SCAND_VIDEO_FILE   = 1,
    SCAND_AUDIO_FILE   = 2,
	SCAND_PICTURE_FILE = 3,
	SCAND_DIR_FILE     = 4,
	SCAND_LINK_FILE    = 5,
}SCAND_FILE_TYPE;

typedef enum{
	SCAND_PF_UNKNOWN = 0,
    SCAND_PF_ISO       = 101,
    SCAND_PF_MKV      = 102,
    SCAND_PF_TS        = 103,
    SCAND_PF_MP4      = 104,
    SCAND_PF_AVI       = 105,
    SCAND_PF_MPG       = 106,
    SCAND_PF_WMV      = 107,
    SCAND_PF_FLV       = 108,
    SCAND_PF_RMVB     = 109,
    SCAND_PF_RM        = 110,

    SCAND_PF_WAV      = 201,
    SCAND_PF_MP3      = 202,
    SCAND_PF_WMA      = 203,
    SCAND_PF_APE      = 204,
    SCAND_PF_ACC      = 205,

	SCAND_PF_BMP      = 301, //Windows 位图 
	SCAND_PF_PNG      = 302, //可移植网络图形 
	SCAND_PF_JPEG     = 303, //联合摄影专家组 
	SCAND_PF_GIF      = 304,  //图形交换格式 
	SCAND_PF_TIFF      = 305, //标记图像文件格式 
	
}SCAND_PACKAGE_FORMAT;


typedef struct{
    char *filecode;                //文件编码
    char *filename;                //文件名称
    char *filepath;                 //文件路径
    unsigned long long int filesize; //文件大小
    SCAND_FILE_TYPE filetype;        //文件类型
    SCAND_PACKAGE_FORMAT filePF;    //文件封装格式   
}SCAND_FILE_INFO;


typedef struct{

	int number;  //影片集数    
    char * videocode;
	SCAND_FILE_INFO file_info;
    void *next;
}SCAND_VIDEO_INFORMATION_LIST;

typedef struct{
    int total_videos;   //影片总集数
    unsigned long long int total_movsize; //影片总大小
    char *movcode;   //影片编码
	SCAND_FILE_INFO file_info;      
    SCAND_VIDEO_INFORMATION_LIST * videolist;
	void *next;
}SCAND_MOVIE_INFORMATION_LIST;


void show_movie_file( SCAND_MOVIE_INFORMATION_LIST * movlist );
int get_serialno_file_from_harddisk( char * mountp, char *ghdsn, int length );
int read_content_from_hdsn_file( char * serialnoPath, char *serialno, int length );
int read_movie_file_from_mbar_hddisk( SCAND_HARDDISK_STYLE style, char *path, SCAND_MOVIE_INFORMATION_LIST ** movlist );



#endif //__SCAND_FILE_H__
