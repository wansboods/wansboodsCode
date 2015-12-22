#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64


#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <pthread.h>

#include <net/if.h>
#include <scsi/sg.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <net/if_arp.h>
#include <arpa/inet.h>
#include <linux/hdreg.h>
#include <sys/socket.h>

#include <scsi/sg.h>
#include <linux/hdreg.h>

#include "scand_file.h"
#include "scand_basiclib.h"
#include "scand_harddisk.h"


#define SCAND_SYSTEM_BLOCK_DEVICE "/sys/block"
#define SCAND_MOUNT_RESULT_FILE "/tmp/mntresult"

#define SERIALNO_FILE_NAME "hdsn.txt"
#define CAPIRALS_SERIALNO_FILE_NAME "HDSN.txt"

#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn    dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug   dzlog_debug
	#define notice  dzlog_notice
#else
//#define print_text(x)       printf(x)
//#define print_frmt(f, x)    printf(f, x)
//#define print_mstr(x)   printf(MT("%s"), x)
    
	#define info printf
	#define warn printf 
    #define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif

#define MAX_SCAND_LINE_LENGTH 1024
#define MAX_SCAND_DEVINFO_LENGTH 512
#define MAX_SCAND_SCSI_TIMEOUT 5000 /* ms */

void free_harddisk_list( SCAND_HD_INFO_LIST ** list );
void free_partition_list( SCAND_PARTITION_INFO_LIST ** list );

SCAND_HD_INFO_LIST * create_harddisk_node( void );
SCAND_PARTITION_INFO_LIST * create_partition_information_node( void );

void add_node_into_hdinfo_list( SCAND_HD_INFO_LIST ** list, SCAND_HD_INFO_LIST * node );
void add_node_into_partition_list( SCAND_PARTITION_INFO_LIST ** list, SCAND_PARTITION_INFO_LIST * node );


char * remove_redundant_characters( char * string, char character );
int scand_newDirectory( char * dir );
int scand_mount_partition_from_harddisk( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char * mountp, int fsfmt );
int scand_unmount_partition_from_harddisk( char * mountp );

int scsi_io( int fd, unsigned char * cdb, unsigned char cdb_size, int xfer_dir, unsigned char * data, unsigned int * data_size, unsigned char * sense, unsigned int * sense_len );
int scsi_inquiry_unit_serial_number( int fd, char * serialno, int length );

int fill_harddisk_info( char *, char *, char *, unsigned long long int , SCAND_STORAGE_MEDIUM , SCAND_HD_INFO_LIST * );
int fill_partition_info( int, char *, char *, char *,char *, SCAND_MOUNT_STATUS, SCAND_HARDDISK_STYLE, char *, SCAND_PARTITION_FS, unsigned long long int, unsigned long long int, unsigned long long int,unsigned long long int, SCAND_PARTITION_INFO_LIST * );
	
int complete_harddisk_information( SCAND_HD_INFO_LIST * hd );
int loading_partitions_info_from_harddisk( char * devname, char *hdserialno, SCAND_MOUNT_STATUS *mstatus, SCAND_PARTITION_INFO_LIST ** list  );

int mountDefault( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint );
int mountNTFS( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint );
int mountExt4( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint );
int mountExt3( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint );

int get_hdserialno( const char* devfile, char* serialno, int length );


static SCAND_HD_INFO_LIST * s_hd_list = NULL;

#ifdef MUTEX
	pthread_mutex_t mutex;//互斥对象     
	pthread_mutexattr_t mutexattr;//互斥对象属性 
#endif


char * show_SCAND_HARDDISK_STYLE( int style ){
    switch( style ){
		case EM_SOURCE_HD:
            return( "原盘" );
        case EM_TARGET_HD:
			return "目标盘";
    }

   	return "未知";
}

char * show_SCAND_MOUNT_STATUS( int status ){
	switch( status ){
		case EM_UNMOUNT:
            return "未挂载";
        case EM_MOUNTED:
            return "已挂载";
    }

    return "未知 ";
}


void show_hd_list(){
	SCAND_HD_INFO_LIST * plist = s_hd_list;
    while( plist ){
		info( "================================\n" );
    	info( "设备路径:%s\n", plist->devfile );
    	info( "设备点:%s\n", plist->devname );
    	info( "硬盘序列号:%s\n", plist->serialno );
		info( "硬盘容量:%llu\n", plist->totalsize );
	    info( "硬盘屏蔽位:%d\n", plist->maskbit );
		info( "存储介质:%d\n", plist->medium );
    	info( "硬盘挂载情况:%d->(%s)\n", plist->mstatus, show_SCAND_MOUNT_STATUS(plist->mstatus) );        
	    SCAND_PARTITION_INFO_LIST * partlist = plist->partlist;
        while( partlist ){
			info( "\t---------\n" );
			info( "\t分区号:%d\n", partlist->index );
			info( "\t分区类型:%d->(%s)\n", partlist->style, show_SCAND_HARDDISK_STYLE( partlist->style ) );
			info( "\t分区挂载情况:%d->(%s)\n", partlist->mstatus, show_SCAND_MOUNT_STATUS( partlist->mstatus ) );
			info( "\t分区设备名:%s\n", partlist->devname );
			info( "\t分区设备路径:%s\n", partlist->devfile );
			info( "\t分区挂载点:%s\n",  partlist->mountpoint );            
            info( "\t分区使用空间:%llu\n", partlist->usedsize  );
            info( "\t分区实际使用空间:%llu\n", partlist->availsize  );
            info( "\t分区可用空间:%llu\n", partlist->freesize  );
            info( "\t分区总空间:%llu\n", partlist->totalsize  );

            info( "\t硬盘设备名:%s\n",  partlist->hddevname );
			info( "\t硬盘设备路径:%s\n",  partlist->hddevfile );

			info( "\t----- end ----\n\n" );		
			
			partlist = partlist->next;
        }

        info( "==============end==================\n\n" );        
        plist = plist->next;
    }
}

int filter_device( char * devname ){    
    if( *devname != 's' ){
        return -1;
    }
    
    return 0;
}

int loading_hard_disk_list( DIR * sock_dir, SCAND_HD_INFO_LIST ** list ){

    int retcode = 0;    
    char devfile[1024] =  { '\0' };
    char devicepath[2048] = { '\0' };

    SCAND_HD_INFO_LIST * scand_hd_node = NULL;    
    struct dirent * ptr = NULL;
    while( ( ptr = readdir( sock_dir ) ) ){
        if( ptr->d_name ){
            if( 0 == filter_device( ptr->d_name ) ){
                sprintf( devicepath, "%s/%s", SCAND_SYSTEM_BLOCK_DEVICE, ptr->d_name );
                struct stat status;
                if( lstat( devicepath, &status ) == 0 ){
                    sprintf( devfile, "/dev/%s", ptr->d_name );
                    char serialno[128] =  { '\0' };
					//获取硬盘序列号
					retcode = get_serialno_from_harddisk( devfile, serialno, sizeof( serialno ) );
					if( 0 > retcode ){
						warn( "( %d ) 获取硬盘序列号出错!\n", retcode );
                        //continue;
                    }				

					unsigned long long int totalsize = 0;
					//获取硬盘大小
					retcode = get_total_size_from_harddisk( devfile, &totalsize );
					if( 0 > retcode ){
						warn( "( %d ) 获取硬盘大小出错!\n", retcode );
                        continue;
                    }
                                       
					SCAND_HD_INFO_LIST * node = create_harddisk_node( );
                    if( NULL == node ){
                        warn( "创建SCAND_HD_INFO_LIST 结构出错!\n" );
                        continue;
                    }

                    add_node_into_hdinfo_list( list, node );
                    retcode = fill_harddisk_info( ptr->d_name, devfile, serialno, totalsize, EM_STORAGE_HD, node );
                    if( retcode != 0 ){
						warn( "注入数据值harddisk info 失败!\n" );
                    }
                    
                    retcode = complete_harddisk_information( node );
                    if( retcode != 0 ){

                    }

                    
                }
            }
        }
    }

    return 0;
}

int start_loading_hard_disk_list( ){
#ifdef MUTEX
	// 初始化互斥对象属性
	pthread_mutexattr_init( &mutexattr );

	// 设置互斥对象:
	//   PTHREAD_PROCESS_SHARED  : 即可以在多个进程的线程访问, 
	//   PTHREAD_PROCESS_PRIVATE : 同一进程的线程共享     
	pthread_mutexattr_setpshared( &mutexattr, PTHREAD_PROCESS_SHARED );
#endif

    DIR * dir = NULL;
    if( NULL == ( dir = opendir( SCAND_SYSTEM_BLOCK_DEVICE ) ) ){
        
        warn( "打开硬盘目录%s 失败..  err:(%s)", SCAND_SYSTEM_BLOCK_DEVICE, strerror( errno ) );
        return -1;
    }

    loading_hard_disk_list( dir, &s_hd_list );    
    closedir( dir );



    show_hd_list();
    return 0;
}


int save_serialno_from_harddisk( int sock, char * serialno, int length ){
	if( NULL == serialno && sizeof( int ) > length ){
		warn( "传入的值出错!" );
		return -1;
    }
    
    int vers = 0;
    if( ( ioctl( sock, SG_GET_VERSION_NUM, &vers ) < 0 ) || ( vers < 30000 ) ){        
        warn( "ioctl 处理出错!" );
        return -1;
    }

    if( scsi_inquiry_unit_serial_number( sock, serialno, length ) != 0 ){
        printf( "scsi 处理出错" );
        return -1;
    }

	return 0;
}

char * remove_redundant_characters( char * string, char character ){
    if( NULL == string ){
		warn( "传入的字符串为 nul" );
        return NULL;
	}
    
    int i = 0;
    int length = strlen( string );
    char * pstring = string;
    for( ; i < length; i++ ){
        if( *pstring == character ){
            *pstring ++ = 0;
        }else{
        	break;
        }
    }

    for( i = length - 1; i >= 0; i -- ){
        if( string[ i ] == character ){
            string[ i ] = 0;
        }
        else{
            break;
        }
    }

    return pstring;
}

int get_serialno_from_harddisk( char * devfile, char * serialno, int length ){
	if( NULL == devfile ){
		warn( "传入值出错!\n" );
        return -1;
    }

    char hd_serialno[ 128 ] = { '\0' };
    int retcode = get_hdserialno( devfile, hd_serialno, sizeof( hd_serialno ) );
    if( retcode != 0 ){
		warn( "获取硬盘序列号出错!\n" );
		return -1;
    }

    char * removeSerialno = remove_redundant_characters( hd_serialno, ' ' );
    //info( ">>>>removeSerialno:%s\n ", removeSerialno );
    if( NULL == removeSerialno ){
		warn( "去除多余字符出错!\n" );
        return -1;
    }

    if( length < strlen( removeSerialno ) + 1 ){
		warn( "存入数据值大小出错!\n" );
        return -1;
    }
    
    memcpy( serialno, removeSerialno, strlen( removeSerialno ) + 1 );
    //info( ">>>>removeSerialno:%s\n ", removeSerialno );

    
#if 0    
    
    int sock;
    if( ( sock = open( devfile, O_RDONLY | O_NONBLOCK ) ) < 0 )
    {
        printf( "Failed to open file %s. err: %s\n", devfile, strerror( errno ) );
        return -1;
    }

    int retcode = 0;
    char hd_serialno[ 128 ] = { '\0' };
        
	if( 0 > ( retcode = save_serialno_from_harddisk( sock, serialno, length ) ) ){
		warn( "(%d) 保存硬盘序列号出错!", retcode );
	    close( sock );
        return -1;
    }

    close( sock );        
    char * removeSerialno = remove_redundant_characters( hd_serialno, ' ' );
    if( NULL == removeSerialno ){
		warn( "去除多余字符出错!" );
        return -1;
    }

    if( length < strlen( removeSerialno ) + 1 ){
		warn( "存入数据值大小出错!" );
        return -1;
    }
    
    memcpy( serialno, removeSerialno, strlen( removeSerialno ) + 1 );
#endif

    return 0;
}
    
int get_total_size_from_harddisk( char * devfile, unsigned long long int * totalsize ){
    if( NULL == devfile ){
        warn( "设备名传入值为nul" );
        return -1;
    }

	char * COMMANDLINE = "parted -s %s print | grep %s | awk '{print $3}'";
    char line[ 1024 ] = { '\0' };        
    char command[1024] = { '\0' };
    if( sizeof( command ) < strlen( COMMANDLINE ) + strlen( devfile ) *2 +1 ){
		warn( "无法存入command 内存" );
        return -1;
    }
    
    sprintf( command, COMMANDLINE, devfile, devfile );
    
    FILE * sock;
    if( NULL == ( sock = popen( command, "r" ) ) ){
        warn( "执行命令( %s ) 出错 err: (%s)", command, strerror( errno ) );
        return -1;
    }

    unsigned long long int size = 0;
    unsigned long long int power = 1;
    if( fgets( line, sizeof( line ), sock ) ){
        if( strstr( line, "GB" ) ){
            power = 1024 * 1024 * 1024;
        }else if( strstr( line, "MB" ) ){
            power = 1024 * 1024;
        }else if( strstr( line, "KB" ) ){
            power = 1024;
        }
        
        sscanf( line, "%llu", &size );
    }

    pclose( sock );
    *totalsize = size * power;    
	return 0;
}

void free_partition_list( SCAND_PARTITION_INFO_LIST ** list ){

    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    SCAND_PARTITION_INFO_LIST * stepnode = *list;
    while( stepnode ){
        SCAND_PARTITION_INFO_LIST * nextnode = stepnode->next;
        free( stepnode );
        stepnode = nextnode;
    }
    
    *list = NULL;
}

void free_harddisk_list( SCAND_HD_INFO_LIST ** list ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    SCAND_HD_INFO_LIST * stepnode = *list;
    while( stepnode ){
        SCAND_HD_INFO_LIST * nextnode = stepnode->next;
        free_partition_list( &stepnode->partlist );
        free( stepnode );
        stepnode = nextnode;
    }
    
    *list = NULL;   
    return;
}

SCAND_PARTITION_INFO_LIST * create_partition_information_node( void ){
    SCAND_PARTITION_INFO_LIST * node;
    if( NULL == ( node = ( SCAND_PARTITION_INFO_LIST * )calloc( sizeof( SCAND_PARTITION_INFO_LIST ), 1 ) ) ){
		warn( "创建node 值为nul" );
        return NULL;
    }
    
    return node;
}

SCAND_HD_INFO_LIST * create_harddisk_node( void ){  
    SCAND_HD_INFO_LIST * node = ( SCAND_HD_INFO_LIST * )calloc( sizeof( SCAND_HD_INFO_LIST ), 1 );
    if( NULL == node ){
		warn( "创建node 值为nul" );
        return NULL;        
	}

    node->next = NULL;
    return node;
}



void add_node_into_hdinfo_list( SCAND_HD_INFO_LIST ** list, SCAND_HD_INFO_LIST * node ){
    if( NULL == list ){
        return;
    }
    
    if( NULL == *list ){
        *list = node;
        return;
    }
    
    SCAND_HD_INFO_LIST * stepnode = *list;
    while( stepnode ){
        if( !stepnode->next ){
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }
}

void add_node_into_partition_list( SCAND_PARTITION_INFO_LIST ** list, SCAND_PARTITION_INFO_LIST * node ){
    if( NULL == list ){
        return;
    }
    
    if( NULL == *list ){
        *list = node;
        return;
    }
    
    SCAND_PARTITION_INFO_LIST * stepnode = *list;
    while( stepnode ){
        if( !stepnode->next ){
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }
}



int complete_harddisk_information( SCAND_HD_INFO_LIST * hd ){

	int retcode = 0;
	SCAND_PARTITION_INFO_LIST * partition_list = NULL;
    retcode = loading_partitions_info_from_harddisk( hd->devname, hd->serialno, &hd->mstatus, &partition_list  );
	if( retcode != 0 ){
		warn( "获取分区( %s )列表信息出错!\n", hd->devname );
        return -1;
    }

	if( NULL == partition_list ){
		warn( " 获取分区列表为nul!\n" );        
        return -1;
    }
#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif 

    if( hd->partlist ){        
        //清空原来的数据      
		free_partition_list( &hd->partlist );		
    }

	hd->partlist = partition_list;

#ifdef MUTEX
	thread_mutex_unlock( &mutex );
#endif

    return 0;    
}

int loading_partitions_list( DIR * sockdir, char *blockpath, char *devname, char *ghdserialno, SCAND_MOUNT_STATUS * mstatus, SCAND_PARTITION_INFO_LIST ** list ){
	if( NULL == devname ){
		warn( "设备名传入出错!\n" );
		return -1;
    }

    int retcode = 0;
    char hdserialno[ 128 ] = { '\0' };    
    struct dirent * ptr;
    while( ( ptr = readdir( sockdir ) ) ){        
        if( ptr->d_name ){
            if( 0 != scand_safe_memcmp( devname, ptr->d_name, strlen( devname ) ) ){
                //warn( "设备名(%s) (%s) 不匹配,跳过..\n", ptr->d_name, devname );
				continue;
            }else{
				warn( "设备名(%s) (%s)  匹配, 进行解析\n", ptr->d_name, devname );	
            }

			int index = 0;
            SCAND_PARTITION_FS fsfmt = EM_PARTITION_FS_UNKNOWN;
            SCAND_MOUNT_STATUS mountstatus = 0;
            SCAND_HARDDISK_STYLE sourceStatus = 0;
                
			unsigned long long int usedsize = 0;                
			unsigned long long int freesize =  0;
			unsigned long long int totalsize = 0;
			unsigned long long int availsize = 0;

			char devfile[ 128 ] = { '\0' };
			char hddevfile[ 128 ] = { '\0' };
		    char filepath[ 1024 ] = { '\0' };
			char mountpoint[ 1024 ] = { '\0' };
			char local_mount_dir[ 1024 ] = { '\0' };
            sprintf( filepath, "%s/%s", blockpath, ptr->d_name );
            //获取分区序号
			retcode = get_partidx_from_harddisk( filepath, &index );
            if( retcode != 0 ){
				warn( "(%d) 获取分区失败(%s),  跳转... \n", retcode, filepath );
                continue;
            }


            //获取分区设备文件,设备号
			sprintf( devfile, "/dev/%s", ptr->d_name );                
            sprintf( hddevfile, "/dev/%s", devname );


            
			//获取分区文件系统格式
			int gfsfmt = 0;
            retcode = get_partition_format_from_harddisk( hddevfile, index, &gfsfmt );
			if( retcode != 0 ){
				warn( "(%d) 获取分区文件系统格式失败(%s)(%d)\n ", retcode, hddevfile, index );
                //continue;
            }
            
            fsfmt = gfsfmt;
            
            //获取分区总大小
			retcode = get_partition_totalsize_from_harddisk( hddevfile, index, &totalsize );
			if( retcode != 0 ){
				warn( "(%d) 获取分区总容量失败(%s)(%d)\n ", retcode, hddevfile, index );
                //continue;
            }
            
            //获取分区挂载状态和挂载点
			retcode = get_partition_mountinfo_from_harddisk( devfile, mountpoint, sizeof( mountpoint ) );
            if( 0 != retcode ){
                info( "分区(%s)处于为挂载状态\n", devfile );
				memset( local_mount_dir, 0, sizeof( local_mount_dir ) );
                //硬盘处于未挂载,程序自动进行挂载硬盘
#ifdef MOUNTED_HDIDSK
				sprintf( local_mount_dir, "%s/%s", SCAND_USER_HD_LOCAL_MOUNT_DIR, ptr->d_name );
				retcode = scand_newDirectory( local_mount_dir );
                if( 0 != retcode ){
					warn( "创建目录失败local_mount_dir:(%s)\n", local_mount_dir );
                }else{
                    retcode = scand_mount_partition_from_harddisk( EM_READ_AND_WIRTE_MODE, devfile, local_mount_dir, fsfmt );
					if( 0 != retcode ){
						warn( "(%d) 挂载出错! (%s)(%s)(%d)\n", retcode, devfile, local_mount_dir, fsfmt );
                        mountstatus = EM_UNMOUNT;
                    }else{
						mountstatus = EM_MOUNTED;
				        memcpy( mountpoint, local_mount_dir, strlen( local_mount_dir ) + 1 );                        
                    }

					retcode = get_source_info_from_harddisk( local_mount_dir, &sourceStatus );
                    if( retcode != 0 ){
						warn( "获取硬盘类型出错!\n" );
                    }

	#ifdef SOURCE_HD_READ_MOUNT
                    retcode = scand_unmount_partition_from_harddisk( local_mount_dir );
                    if( retcode != 0 ){
						warn( "为能卸载硬盘挂载点( %s )\n", local_mount_dir );
                    }else{
 						mountstatus = EM_UNMOUNT;
                    }
                    
                    // 原盘挂载成只读形式
                    if( sourceStatus == EM_SOURCE_HD ){
						if( mountstatus == EM_MOUNTED ){
                            warn( "硬盘(%s)  处于挂载状态\n", hddevfile  );
                        }
                        
                    	retcode = scand_mount_partition_from_harddisk( EM_OLNY_READ_MODE, devfile, local_mount_dir, fsfmt );
						if( 0 != retcode ){
							warn( "(%d) 挂载出错! (%s)(%s)(%s)\n", devfile, local_mount_dir, fsfmt );
	                        mountstatus = EM_UNMOUNT;
	                    }else{
							mountstatus = EM_MOUNTED;
					        memcpy( mountpoint, local_mount_dir, strlen( local_mount_dir ) + 1 );                        
	                    }
                    }
	#endif                                    
                }

#endif                
            }else{
				mountstatus = EM_MOUNTED;
                *mstatus = EM_MOUNTED;
				retcode = get_source_info_from_harddisk( local_mount_dir, &sourceStatus );
                if( retcode != 0 ){
					warn( "获取硬盘类型出错!\n" );
                }
                    
            }

			if( EM_MOUNTED == mountstatus ){
				//获取分区文件系统容量信息
				retcode = get_partition_filesystem_info_from_harddisk( mountpoint, &usedsize, &availsize, &freesize );
                if( retcode != 0 ){
					warn( "获取硬盘容量信息失败!\n" );
                }else{
					usedsize = totalsize - freesize;
                }

	            //info( ">>>> 当前分区号:%d\n", index );
	            //info( ">>>> 分区设备名:%s\n", devfile );
	            //info( ">>>> 硬盘设备名:%s\n", hddevfile );            
	            //info( ">>>> 分区文件系统%d\n", fsfmt );
	            //info( ">>>> totalsize:%llu \n", totalsize );
	            //info( ">>>> usedsize :%llu \n", usedsize );
	            //info( ">>>> freesize:%llu \n", freesize );
	            //info( ">>>> availsize:%llu \n", availsize );
            				
                //查找特殊文件 hdsn.txt 
                retcode = get_serialno_file_from_harddisk( mountpoint, hdserialno, sizeof( hdserialno ) );
                if( 0 != retcode ){
					warn( "分区下(%s) 未发现有hdsn.txt  文件\n", mountpoint );
                }else{
                	info( "分区下(%s) 存在hdsn.txt , 硬盘sn 改名为: %s\n", mountpoint, hdserialno );
#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif                    
                    if( ghdserialno ){
						scand_safe_free( ghdserialno );
                    }

                    if( hdserialno ){
						ghdserialno = scand_safestrdup( hdserialno );  
                    }
#ifdef MUTEX
	pthread_mutex_unlock( &mutex );
#endif                    
                }    

				SCAND_MOVIE_INFORMATION_LIST * movlist = NULL;
                //获取硬盘分区下的影片信息				
				retcode = read_movie_file_from_mbar_hddisk( sourceStatus, mountpoint, &movlist );
                if( retcode != 0 ){
					warn( "获取硬盘分区下的影片信息失败! " );
                }

                
            }

            SCAND_PARTITION_INFO_LIST * partition_node = create_partition_information_node();
            if( NULL == partition_node ){
				warn( "创建SCAND_PARTITION_INFO_LIST 出错!" );
                continue;
            }

            add_node_into_partition_list( list, partition_node );
			retcode = fill_partition_info( index, ptr->d_name, devfile, devname, hddevfile 
                ,mountstatus, sourceStatus, mountpoint, fsfmt, totalsize, usedsize, availsize,
                freesize, partition_node );
            if( retcode ){
				warn( "注入分区数据出错!" );
            }
            

        }
    }

    return 0;
}
    
int loading_partitions_info_from_harddisk( char * devname, char *hdserialno, SCAND_MOUNT_STATUS * mstatus, SCAND_PARTITION_INFO_LIST ** list  ){
    char blockpath[ 1024 ] = { '\0' };
    sprintf( blockpath, "%s/%s", "/sys/block", devname );
 
    DIR * sockdir;
    if( NULL == ( sockdir = opendir( blockpath ) ) ){        
        warn( "打开目录( %s )出错err: ( %s )\n", blockpath, strerror( errno ) );
        return -1;
    }

    loading_partitions_list( sockdir, blockpath, devname, hdserialno, mstatus, list );
    closedir( sockdir );
    return 0;
}


int get_partidx_from_harddisk( char * filepath, int *gindex ){
	if( NULL == filepath ){
		warn( "传入值出错!\n" );
		return -1;
    }

	char * COMMANDLINE = "%s/partition";
    char partidx_file[ 1024 ] = { '\0' };
    if( sizeof( COMMANDLINE ) +  strlen( COMMANDLINE ) + strlen( filepath ) +  1 > sizeof( partidx_file ) ){
		warn( "无法写入数据值过大!\n" );
		return -1;
    }

    sprintf( partidx_file, COMMANDLINE, filepath );    
	info( "执行命令:%s\n", partidx_file );
    FILE * sock;
    if( NULL == ( sock = fopen( partidx_file, "r" ) ) ){
        
        warn( "Failed to open file %s. err: %s\n", partidx_file, strerror( errno ) );
        return -1;
    }
    
    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
    int readlen = 0;
    if( ( readlen = fread( line, sizeof( line ), 1, sock ) ) < 0 ){        
        warn( "Failed to read data from file %s. err: %s\n", partidx_file, strerror( errno ) );
        *gindex = -1;
    }else{
		*gindex = atoi( line );
    }
        
    fclose( sock );
    return 0;
}

int get_partition_format_from_harddisk( char * devfile, int index, int * fsfmt ){
    if( NULL == devfile ){
        warn( "传入值出错!" );
		return -1;
    }
    
    char cmdline[1204]={ '\0' };
    char * COMMANDLINE = "parted -s %s print | awk '{if( $1 == %d ) print $5 \" \" $6}'";
    if( strlen( COMMANDLINE ) + strlen( devfile ) + sizeof( int ) + 1 > sizeof( cmdline ) ){

        warn( "无法写入数据值过大!" );
		return -1;
    }
    
    sprintf( cmdline, COMMANDLINE, devfile, index );
	info( "执行命令:%s\n", cmdline );    
    FILE * fp;
    if( NULL == ( fp = popen( cmdline, "r" ) ) ){
        warn( "打开文件失败%s, err: %s\n", cmdline, strerror( errno ) );
        return -1;
    }

    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
    if( fgets( line, sizeof( line ), fp ) ){
        
        if( strstr( line, "ext4" ) ){
            *fsfmt = EM_PARTITION_FS_EXT4;
        }else if( strstr( line, "ext3" ) ){
            *fsfmt = EM_PARTITION_FS_EXT3;
        }else if( strstr( line, "ntfs" ) ){
            *fsfmt = EM_PARTITION_FS_NTFS;
        }else if( strstr( line, "fat32" ) ){
			*fsfmt = EM_PARTITION_FS_NTFS;
        }
    }

    pclose( fp );
    return 0;
}  


int get_partition_totalsize_from_harddisk( char * devfile, int index, unsigned long long int * totalsize ){
    char cmdline[ 1024 ] = { '\0' };
    char * COMMANDLINE = "parted -s %s print | awk '{if( $1 == %d ) print $4}'";
    if( strlen( COMMANDLINE ) + strlen( devfile ) + sizeof( int ) + 1 > sizeof( cmdline ) ){
        warn( "无法写入数据值过大!" );
		return -1;
    }
    
    sprintf( cmdline, COMMANDLINE, devfile, index );
	info( "执行命令:%s\n", cmdline );    
    FILE * fp;
    if( NULL == ( fp = popen( cmdline, "r" ) ) ){
        warn( "打开文件失败%s, err: %s", cmdline, strerror( errno ) );
        return -1;
    }

    unsigned long long int size = 0;
    unsigned long long int power = 1;
    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
    if( fgets( line, sizeof( line ), fp ) ){
        
        if( strstr( line, "GB" ) ){
            power = 1000 * 1000 * 1000;
        }else if( strstr( line, "MB" ) ){
            power = 1000 * 1000;
        }else if( strstr( line, "KB" ) ){
            power = 1000;
        }
        
        sscanf( line, "%llu", &size );
    }

    pclose( fp );
    *totalsize = size * power;
    return 0;
}

int get_partition_mountinfo_from_harddisk( char * devfile, char * mountpoint, int length ){

	char * COMMANDLINE = "/etc/mtab";
    FILE *fp = fopen( COMMANDLINE, "r" );
    if( NULL == fp ){
        warn( "打开文件失败%s, err: %s", COMMANDLINE, strerror( errno ) );
        return -1;
    }

	int retcode = 0;
    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
    while( fgets( line, sizeof( line ), fp ) ){
		char mntdir[ 512 ] = { '\0' };
		char filepath[ 1024 ] = { '\0' };
        if( ( retcode = sscanf( line, "%s%s%*s%*s%*s%*s", filepath, mntdir ) ) == 2 ){
            //info( "filepath(%s)mntdir(%s) vs devfile(%s)\n", filepath, mntdir, devfile  );
            if( scand_safe_memcmp( devfile, filepath, strlen( filepath ) ) == 0 ){
                if( strlen( mntdir ) + 1 > length ){
                    warn( "buffer size %d less than data size %ld \n", length, strlen( mntdir ) + 1 );
                }else{
                    memcpy( mountpoint, mntdir, strlen( mntdir ) + 1 );
                    fclose( fp );
                    return 0;
                }
            }
        }
    }
    
    fclose( fp );
    return -1;
}


int scand_newDirectory( char * dir ){
    if( NULL == dir ){
        warn( "传入值出错!" );
        return -1;
    }

    char * topdir = dir;
    char * zonedir = dir;
    char * splitdir = NULL;    
    while( NULL != ( splitdir = strchr( zonedir, '/' ) ) ){
        char localdir[ MAX_SCAND_DEVINFO_LENGTH ] = { '\0' };
        if( ( splitdir - topdir ) ){
            
            memset( localdir, 0, MAX_SCAND_DEVINFO_LENGTH );
            memcpy( localdir, topdir, splitdir - topdir );

            //printf( "create directory: %s\n", localdir );
            if( mkdir( localdir, 0777 ) < 0 ){
                if( errno != EEXIST ){
                    warn( "创建目录%s 失败.. err: %s", localdir, strerror( errno ) );
                    return -1;
                }
            }
        }
        
        zonedir = splitdir + 1;
    }

    if( mkdir( topdir, 0777 ) < 0 ){
        if( errno != EEXIST ){
            warn( "创建目录%s 失败.. err: %s", topdir, strerror( errno ) );            
            return -1;
        }
    }

    return 0;
}




int scand_ExecuteCommand( char * command ){
    if( NULL == command ){
		warn( "传入的参数出错!" );
        return -1;
    }
    
    if( system( command ) < 0 ){
        
        return -1;
    }
    
    return 0;
}

int mountExt3( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint ){
    if( NULL == devfile || NULL == mountpoint ){
       warn( "传入参数出错!" );
       return -1;
    }

    int retcode = 0;
    char command[ 1024 ] = { '\0' };
    memset( command, '\0', sizeof( command ) );     
    if( mode == EM_READ_AND_WIRTE_MODE ){    
        sprintf( command, "mount -o noatime -t ext3 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
		info( "执行命令:%s\n", command );
        retcode = scand_ExecuteCommand( command );
        if( retcode != 0 ){
			warn( "挂载命令出错!" );            
        }
             
    }else if( mode == EM_OLNY_READ_MODE ){
        sprintf( command, "mount -o noatime,ro -t ext3 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
		info( "执行命令:%s", command );
        retcode = scand_ExecuteCommand( command );        
        if( retcode != 0 ){
			warn( "授权命令出错!" );            
        }            
    }

    sprintf( command, "chmod -R 777 %s", mountpoint );
	info( "执行命令:%s", command );

#ifdef EMPOWER_PARTITION    
    retcode = scand_ExecuteCommand( command );        
    if( retcode != 0 ){
		warn( "授权命令出错!" );            
    }   
#endif

    return 0;
}

int mountExt4( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint ){
    if( NULL == devfile || NULL == mountpoint ){
       warn( "传入参数出错!" );
       return -1;
    }

    int retcode = 0;
    char command[ 1024 ] = { '\0' };
    memset( command, '\0', sizeof( command ) );     
    if( mode == EM_READ_AND_WIRTE_MODE ){    
        sprintf( command, "mount -o noatime -t ext4 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
		info( "执行命令:%s\n", command );
        retcode = scand_ExecuteCommand( command );
        if( retcode != 0 ){
			warn( "挂载命令出错!" );            
        }
             
    }else if( mode == EM_OLNY_READ_MODE ){
        sprintf( command, "mount -o noatime,ro -t ext4 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
		info( "执行命令:%s\n", command );
        retcode = scand_ExecuteCommand( command );        
        if( retcode != 0 ){
			warn( "授权命令出错!" );            
        }            
    }

    sprintf( command, "chmod -R 777 %s", mountpoint );
	info( "执行命令:%s\n", command );

#ifdef EMPOWER_PARTITION    
    retcode = scand_ExecuteCommand( command );        
    if( retcode != 0 ){
		warn( "授权命令出错!" );            
    }   
#endif

    return 0;
}


int mountNTFS( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint ){
    if( NULL == devfile || NULL == mountpoint ){
       warn( "传入参数出错!" );
       return -1;
    }

    int retcode = 0;
    char command[ 1024 ] = { '\0' };
    if( mode == EM_READ_AND_WIRTE_MODE ){
        sprintf( command, "ntfsmount -o force,norelatime %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
    }else if( mode == EM_OLNY_READ_MODE ){
        sprintf( command, "ntfsmount -o force,norelatime,ro %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
    }

    
	info( "执行命令:%s\n", command );
    retcode = scand_ExecuteCommand( command );
    if( retcode != 0 ){
		warn( "挂载命令出错!" );            
    }

    sprintf( command, "chmod -R 777 %s", mountpoint );
	info( "执行命令:%s\n", command );

#ifdef EMPOWER_PARTITION
    retcode = scand_ExecuteCommand( command );        
    if( retcode != 0 ){
		warn( "授权命令出错!" );            
    }  
#endif


    return 0;
}
    
int mountDefault( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char *mountpoint ){
    if( NULL == devfile || NULL == mountpoint ){
       warn( "传入参数出错!\n" );
       return -1;
    }

    int retcode = 0;
    char command[ 1024 ] = { '\0' };
    if( mode == EM_OLNY_READ_MODE ){
        sprintf( command, "mount -o ro iocharset=utf8 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
    }else if( mode == EM_READ_AND_WIRTE_MODE ){
        sprintf( command, "mount -o iocharset=utf8 %s %s > /dev/null 2>&1;echo $? > %s", devfile, mountpoint, SCAND_MOUNT_RESULT_FILE );
    }

	info( "执行命令:%s\n", command );
    retcode = scand_ExecuteCommand( command );
    if( retcode != 0 ){
		warn( "挂载命令出错!" );            
    }
    
    sprintf( command, "chmod -R 777 %s", mountpoint );
	info( "执行命令:%s\n", command );
#ifdef EMPOWER_PARTITION    
    retcode = scand_ExecuteCommand( command );        
    if( retcode != 0 ){
		warn( "授权命令出错!" );            
    }
#endif

    return 0;
}


int scand_mount_partition_from_harddisk( SCAND_HARDDISK_MOUNT_MODE mode, char * devfile, char * mountp, int fsfmt ){
    if( NULL == devfile || NULL == mountp ){
        warn( "传入值出错!\n" );
        return -1;
    }

	char cmdline[ 1024 ] = { '\0' };
    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
    info( " 识别到服务器内硬盘( %s ) 未挂载\n", devfile );
	switch( fsfmt ){
        case EM_PARTITION_FS_EXT3:        
			info( "挂载点( %s ), 处理挂载方式为%s", mountp, "EXT3" );
            mountExt3( mode, devfile, mountp );
            break;
        case EM_PARTITION_FS_EXT4:
			info( "挂载点( %s ), 处理挂载方式为%s", mountp, "EXT4" );
            mountExt4( mode, devfile, mountp );
	        break;
        case EM_PARTITION_FS_NTFS:
			info( "挂载点( %s ), 处理挂载方式为%s", mountp, "NTFS" );
            mountNTFS( mode, devfile, mountp );
            break;
        default:
            info( "挂载点( %s ), 处理挂载方式为%s", mountp, "DEFAULT" );
            mountDefault( mode, devfile, mountp );
	}
    
	FILE * fp;    
	if( NULL == ( fp = fopen( SCAND_MOUNT_RESULT_FILE, "r" ) ) ){
        warn( "打开文件失败%s, err: %s", cmdline, strerror( errno ) );
	}else{
		if( !fgets( line, sizeof( line ), fp ) ){		
			warn( "读取内容失败, err:%s", strerror( errno ) ); 
		}
    }
		
	fclose( fp );
	if( *line != '0' ){	
		return 0;
	}
 
    return -1;
}

int scand_unmount_partition_from_harddisk( char * mountp ){
	if( NULL == mountp ){
        warn( "传入值出错!" );
		return -1;
    }

	char command[ 1024 ] = { '\0' };
    char line[ MAX_SCAND_LINE_LENGTH ] = { '\0' };
	sprintf( command, "umount %s", mountp );
	info( "执行命令:%s", command );
    int retcode = scand_ExecuteCommand( command );
    if( retcode != 0 ){
		warn( "(%d) 挂载命令出错!", retcode );
        return -1;
    }

    return 0;
}
    
int get_source_info_from_harddisk( char *mountp, SCAND_HARDDISK_STYLE *sourceStatus ){
    if( NULL == mountp ){
        warn( "传入参数出错!" );
		return -1;
    }
    
    char sourcePath[ 1024 ] = { '\0' };
    sprintf( sourcePath, "%s/%s", mountp, "serial.no" );    
    if( access( sourcePath, R_OK ) == 0 ){
		*sourceStatus = EM_SOURCE_HD;
        return 0;
    }else{
		*sourceStatus = EM_TARGET_HD;
        return 0;
    }
    
    sprintf( sourcePath, "%s/%s", mountp, "SERIAL.NO" );
    if( access( sourcePath, R_OK ) == 0 ){
		*sourceStatus = EM_SOURCE_HD;
        return 0;
    }else{
		*sourceStatus = EM_TARGET_HD;
        return 0;
    }
    
    return -1;
}


int get_partition_filesystem_info_from_harddisk( char * mountp, unsigned long long int * usedsize, unsigned long long int * availsize, unsigned long long int * freesize ){

	if( NULL == mountp ){
		warn( "传入的参数出错!" );
		return -1;
    }
    
    struct statfs disk_statfs;
    if( statfs( mountp, &disk_statfs ) < 0 ){
        warn( "执行statfs 函数报错,err:%s ", strerror( errno ) );
        return -1;
    }
    
    *usedsize = 0;
    *availsize = ( unsigned long long int )disk_statfs.f_bsize * disk_statfs.f_bavail;
    *freesize = ( unsigned long long int )disk_statfs.f_bsize * disk_statfs.f_bfree;
    return 0;
}

int fill_harddisk_info( char * devname, char * devfile, char * serialno,
    unsigned long long int totalsize, SCAND_STORAGE_MEDIUM medium,
    SCAND_HD_INFO_LIST * hdnode ){

    if( NULL == hdnode ){
		warn( "传入值出错!" );
		return -1;
    }

#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif

    hdnode->partlist = NULL;
    hdnode->medium = medium;
    hdnode->totalsize = totalsize;
    hdnode->devname = scand_safestrdup( devname );
	hdnode->devfile = scand_safestrdup( devfile );
    hdnode->serialno = scand_safestrdup( serialno ); 
    //info( ">>>> devfile(%s) 获取设备号:%s\n", hdnode->devfile, hdnode->serialno );
    

#ifdef MUTEX
	pthread_mutex_unlock( &mutex );
#endif
	return 0;
}


int fill_partition_info( int index, char * devname, char * devfile, char * hddevname,
    char * hddevfile, SCAND_MOUNT_STATUS mountstatus, SCAND_HARDDISK_STYLE style, 
    char * mountpoint, SCAND_PARTITION_FS fsfmt, unsigned long long int totalsize,
    unsigned long long int usedsize, unsigned long long int availsize,unsigned long long int freesize,
	SCAND_PARTITION_INFO_LIST * partition_node ){

    if( NULL == partition_node ){
		warn( "传入值出错!" );
		return -1;
    }
#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif
    partition_node->index = index; //分区号
   	partition_node->style = style; //分区类型 ( 原盘/目标盘 )
   	partition_node->mstatus = mountstatus; //分区挂载情况

    partition_node->usedsize = usedsize;
    partition_node->availsize = availsize;
    partition_node->freesize = freesize;
    partition_node->totalsize = totalsize;

    partition_node->devname = scand_safestrdup( devname );
    partition_node->devfile = scand_safestrdup( devfile );
    partition_node->hddevname = scand_safestrdup( hddevname );
    partition_node->hddevfile = scand_safestrdup( hddevfile );
    partition_node->mountpoint = scand_safestrdup( mountpoint );

#ifdef MUTEX
	pthread_mutex_unlock( &mutex );
#endif

    return 0;
}

int get_serialno_file_from_harddisk( char * mountp, char *serialno, int length ){

	if( NULL == mountp ){
		warn( "传入的值出错!\n" );
        return -1;
    }

    int retcode = 0;
	char serialnoPath[ 1024 ] = { '\0' };
    sprintf( serialnoPath, "%s/%s", mountp, CAPIRALS_SERIALNO_FILE_NAME );
	info( "查找路径是: %s\n", serialnoPath );    
	if( access( serialnoPath, R_OK ) == 0 ){
		retcode = read_content_from_hdsn_file( serialnoPath, serialno, length );
        if( 0 != retcode ){
			warn( "读取内容出错，错误码:(%d)\n", retcode );
            return -1;
        }else{
			return 0;
        }
    }

    sprintf( serialnoPath, "%s/%s", mountp, SERIALNO_FILE_NAME );
	info( "查找路径是: %s\n", serialnoPath );
    if( access( serialnoPath, R_OK ) == 0 ){
		retcode = read_content_from_hdsn_file( serialnoPath, serialno, length );
        if( 0 != retcode ){
			warn( "读取内容出错，错误码:(%d)\n", retcode );
            return -1;
        }else{
			return 0;
        }
    }

    return -1;   
}



int scsi_inquiry_unit_serial_number( int fd, char * serialno, int length ){
    unsigned char cdb[] = { 0x12,0x01,0x80, 0, 0, 0 };
    unsigned int data_size = 0x00ff;
    unsigned char data[ data_size ];
    unsigned int sense_len = 32;
    unsigned char sense[ sense_len ];
    int res, pl, i;
    cdb[3] = ( data_size>>8 )&0xff;
    cdb[4] = data_size&0xff;
    res = scsi_io( fd, cdb, sizeof( cdb ), SG_DXFER_FROM_DEV, data, &data_size, sense, &sense_len );
    if( res )
    {
        printf( "SCSI_IO failed\n" );
        return -1;
    }
    if( sense_len )
    {
        return -1;
    }

    /* Page Length */
    pl = data[3];
    int offset = 0;
    /* Unit Serial Number */
    for( i = 4; i < ( pl + 4 ); i++ )
    {
        sprintf( serialno + offset, "%c", data[i]&0xff );
        offset++;
    }
    return 0;
}


int scsi_io( int fd, unsigned char * cdb, unsigned char cdb_size, int xfer_dir, unsigned char * data, unsigned int * data_size, unsigned char * sense, unsigned int * sense_len ){
    sg_io_hdr_t io_hdr;
    memset( &io_hdr, 0, sizeof( sg_io_hdr_t ) );
    io_hdr.interface_id = 'S';

    /* CDB */
    io_hdr.cmdp = cdb;
    io_hdr.cmd_len = cdb_size;

    /* Where to store the sense_data, if there was an error */
    io_hdr.sbp = sense;
    io_hdr.mx_sb_len = *sense_len;
    *sense_len = 0;

    /* Transfer direction, either in or out. Linux does not yet
 *      * support bidirectional SCSI transfers ?
 *           */
    io_hdr.dxfer_direction = xfer_dir;

    /* Where to store the DATA IN/OUT from the device and how big the
 *      * buffer is
 *           */
    io_hdr.dxferp = data;
    io_hdr.dxfer_len = *data_size;

    /* SCSI timeout in ms */
    io_hdr.timeout = MAX_SCAND_SCSI_TIMEOUT;

    if( ioctl( fd, SG_IO, &io_hdr ) < 0 )
    {
        perror("SG_IO ioctl failed");
        return -1;
    }

    /* now for the error processing */
#if 0   
    if( ( io_hdr.info & SG_INFO_OK_MASK ) != SG_INFO_OK )
    {
        if( io_hdr.sb_len_wr > 0 )
        {
            *sense_len = io_hdr.sb_len_wr;
            return 0;
        }
    }
#endif

    if( io_hdr.masked_status )
    {
        printf( "masked_status=0x%x\n", io_hdr.masked_status);
        return -2;
    }

    if( io_hdr.host_status )
    {
        printf( "host_status=0x%x\n", io_hdr.host_status);
        return -3;
    }

    if( io_hdr.driver_status )
    {
        printf( "driver_status=0x%x\n", io_hdr.driver_status);
        return -4;
    }

    return 0;
}


int get_hdserialno( const char* devfile, char* serialno, int length ){
    if( NULL == devfile ){
		warn( "传入值出错!" );
		return -1;
    }
    
    struct hd_driveid id;
    int retcode = -1;
	int  fd = open( devfile , O_RDONLY|O_NONBLOCK );
    while( 1 ){
        if ( fd < 0 ){
			warn( "读取内容失败, err:%s", strerror( errno ) );             
            break;
        }

        if( !ioctl( fd, HDIO_GET_IDENTITY, &id ) ){
             strncpy( serialno, id.serial_no, length );
             info( "serialno:%s\n", serialno  );
             retcode = 0;
        }
        
        break;
    }
    
    return retcode;
}



