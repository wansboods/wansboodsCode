#ifndef __SCAND_HARDDISK_H_
#define __SCAND_HARDDISK_H_

typedef enum{

	EM_OLNY_READ_MODE = 1,
    EM_READ_AND_WIRTE_MODE = 2,

}SCAND_HARDDISK_MOUNT_MODE;


typedef enum{
    
    EM_UNKNOWN = 0, 
    EM_SOURCE_HD = 1,    //原盘
    EM_TARGET_HD = 2,    //目标盘
    EM_DOWNLOAD_HD = 3, //下载盘
    
}SCAND_HARDDISK_STYLE;


typedef enum{

	EM_MASK_OPEN  = 0,
    EM_MASK_CLOSE = 1,
    
}SCAND_HD_MASKBIT;

typedef enum{
    
    EM_STORAGE_HD = 1,
    EM_STORAGE_CD = 2,
    EM_STORAGE_SSD = 3,
    EM_STORAGE_UNKNOWN = 0,    
}SCAND_STORAGE_MEDIUM;

typedef enum{    
    EM_UNMOUNT = 0,
    EM_MOUNTED = 1,
}SCAND_MOUNT_STATUS;

typedef enum
{
    EM_PARTITION_FS_UNKNOWN  = 0,
    EM_PARTITION_FS_NTFS       = 1,
    EM_PARTITION_FS_FAT32      = 2,
    EM_PARTITION_FS_EXT3       = 3,
    EM_PARTITION_FS_EXT4       = 4,
    EM_PARTITION_FS_UNSUPPORT = 5,    
}SCAND_PARTITION_FS;


typedef struct{
    int index;                       //分区号
    SCAND_HARDDISK_STYLE style;       //分区类型 ( 原盘/目标盘 )
    SCAND_MOUNT_STATUS mstatus;      //分区挂载情况
    unsigned long long int usedsize;
    unsigned long long int freesize;
    unsigned long long int availsize;
    unsigned long long int totalsize;

	void * list;                   //存储点

    char * devname;              //分区设备名
    char * devfile;               //分区设备路径    
    char * mountpoint;            //挂载点

    char * hddevname;            //硬盘设备名
    char * hddevfile;              //硬盘设备路径
    
    void * next;
}SCAND_PARTITION_INFO_LIST;

typedef struct{

    char *devfile;                      //设备路径
    char *devname;                    //设备点
    char *serialno;                     //硬盘序列号
    long hdcapacity;                    //硬盘容量
    unsigned long long int totalsize;    //硬盘容量

    SCAND_HD_MASKBIT maskbit;          //屏蔽标志
    SCAND_STORAGE_MEDIUM medium;      //存储介质    
    SCAND_MOUNT_STATUS mstatus;        //硬盘挂载情况
    
    SCAND_PARTITION_INFO_LIST * partlist;
    void *next;
}SCAND_HD_INFO_LIST;

#define SCAND_USER_HD_LOCAL_MOUNT_DIR  "/media"

int start_loading_hard_disk_list( SCAND_HD_INFO_LIST ** list );

int get_total_size_from_harddisk( char * devfile, unsigned long long int * totalsize );
int get_serialno_from_harddisk( char * devfile, char * serialno, int length );


int get_partidx_from_harddisk( char * filepath, int *gindex );
int get_partition_format_from_harddisk( char * devfile, int index, int * fsfmt );
int get_partition_totalsize_from_harddisk( char * devfile, int index, unsigned long long int * totalsize );
int get_partition_mountinfo_from_harddisk( char * devfile, char * mountpoint, int length );
int get_source_info_from_harddisk( char *mountp, SCAND_HARDDISK_STYLE *sourceStatus );
int get_partitions_info_from_harddisk( char * blockpath, SCAND_PARTITION_INFO_LIST ** list );
int get_partition_filesystem_info_from_harddisk( char * mountp, unsigned long long int * usedsize, unsigned long long int * availsize, unsigned long long int * freesize );
int get_serialno_file_from_harddisk( char * mountp, char *serialno, int length );
int get_partition_mountmode_from_harddisk( char * devfile, SCAND_HARDDISK_MOUNT_MODE * mode );

#endif //__SCAND_HARDDISK_H_
