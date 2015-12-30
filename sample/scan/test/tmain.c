#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>

#include "scand_file.h"
#include "tread_config.h"
#include "scand_basiclib.h"
#include "scand_harddisk.h"


#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn    dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug   dzlog_debug
	#define notice  dzlog_notice
#else
	#define info   printf
	#define warn   printf
	#define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif


int find_filename_from_buf( char * filename, char * toname, int length, READCFN_CONFIG_LIST * list ){
	while( list ){
        if( 0 == scand_safe_memcmp( list->l_value, filename, strlen( filename ) ) ){
			if( list->r_value ){
                if( length > strlen( list->r_value ) + 1 ){
					memcpy( toname, list->r_value, strlen( list->r_value ) + 1 );
                	return 0;
                }
                else{
                    warn( "传入的buf 值过小, 不满足存放值需要的空间( %ld )\n", strlen( list->r_value ) + 1 );
                }
            }else{
				warn( "配置文件标识符(%s)右边值为nul..\n", list->l_value );
            }
        }        
            
		list = list->next;
    }

    return -1;
}

int main( int argc, char *argv[] ){
    SCAND_HD_INFO_LIST * hdlist = NULL;
	start_loading_hard_disk_list( &hdlist );
    
	READCFN_CONFIG_LIST * cfnlist = NULL;
    cfn_read_config( "movlist.txt", &cfnlist );
    
#if 0
　　if( rename(argv[1],argv[2] < 0 )
　　	printf(“error!\n”);
　　else
　　	printf(“ok!\n”);
#endif

	int retcode = 0;
    SCAND_HD_INFO_LIST * phdlist = hdlist;
    while( phdlist ){
#if 1  
		if( phdlist->mstatus != EM_MOUNTED ){
			info( "硬盘序列号( %s ) 设备号( %s ) 未挂载,忽略.. \n", phdlist->serialno, phdlist->devfile );
            phdlist = phdlist->next;
            continue;
        }
#endif

#if 0
		info( "================================\n" );
    	info( "设备路径:%s\n", phdlist->devfile );
    	info( "设备点:%s\n", phdlist->devname );
    	info( "硬盘序列号:%s\n", phdlist->serialno );
		info( "硬盘容量:%llu\n", phdlist->totalsize );
	    info( "硬盘屏蔽位:%d\n", phdlist->maskbit );
		info( "存储介质:%d\n", phdlist->medium );
        info( "硬盘挂载情况:%dn",  phdlist->mstatus );
#endif        
    	//info( "硬盘挂载情况:%d->(%s)\n", phdlist->mstatus, show_SCAND_MOUNT_STATUS(phdlist->mstatus) );        
	    SCAND_PARTITION_INFO_LIST * partlist = phdlist->partlist;
        while( partlist ){
#if 1             
            if( partlist->mstatus != EM_MOUNTED ){
                info( "硬盘序列号( %s )  分区号( %d )  分区设备名( %s ) 未挂载,忽略..\n", phdlist->serialno, partlist->index, partlist->devname );
				partlist = partlist->next;
                continue;
            }
#endif  
			#if 0
			info( "\t---------\n" );
			info( "\t分区号:%d\n", partlist->index );
            info( "\t分区类型:%d\n", partlist->style );
			info( "\t分区挂载情况:%d\n", partlist->mstatus  );
			info( "\t分区设备名:%s\n", partlist->devname );
			info( "\t分区设备路径:%s\n", partlist->devfile );
			info( "\t分区挂载点:%s\n",  partlist->mountpoint );            
            info( "\t分区使用空间:%llu\n", partlist->usedsize  );
            info( "\t分区实际使用空间:%llu\n", partlist->availsize  );
            info( "\t分区可用空间:%llu\n", partlist->freesize  );
            info( "\t分区总空间:%llu\n", partlist->totalsize  );

            info( "\t硬盘设备名:%s\n",  partlist->hddevname );
			info( "\t硬盘设备路径:%s\n",  partlist->hddevfile );
			#endif
            
            SCAND_MOVIE_INFORMATION_LIST * movlist = ( SCAND_MOVIE_INFORMATION_LIST * ) partlist->list;   
			while( movlist ){
				char toname[ 512 ] = { '\0' };
                if( movlist->file_info.filename ){
					retcode = find_filename_from_buf( movlist->file_info.filename, toname, sizeof( toname ), cfnlist );
					if( retcode == 0 ){
						info(  "硬盘序列号:%s", phdlist->serialno );
                        info(  " 分区号( %d )  分区设备名( %s )  分区挂载点( %s ) 文件名( %s) \n", partlist->index, partlist->devname, partlist->mountpoint, movlist->file_info.filename );
						info(  "找到对应的关系, 需要将文件名( %s )  改名成( %s )\n", movlist->file_info.filename, toname );
                        
						char path[ 512 ] = { '\0' };
                        char suffix[ 32 ] = { '\0' };
						retcode = get_path_and_suffix_form_filepath( movlist->file_info.filepath, path, sizeof( path ), suffix, sizeof( suffix ) );
						if( retcode == 0 ){                        
							char newfilepath[ 1024 ] = { '\0' };
							sprintf( newfilepath, "%s/%s%s", path, toname, suffix );
                            info( "新名称:%s\n", newfilepath );
							#if 1 
								if( rename( movlist->file_info.filepath, newfilepath ) < 0 ){
									warn( "改名失败!\n" );
                                }else{
									info( "改名成功!\n" );
									char serialpath[ 512 ] = { '\0' };
                                    sprintf( serialpath, "%s/%s", partlist->mountpoint, "serial.no" );
                                    info( "serialpath:%s\n", serialpath );
                                    if( access( serialpath, R_OK ) == 0 ){
										info( "serialpath(%s) 文件存在,无需创建!", serialpath );
                                    }else{                                    
										int fd;									
										fd = open( serialpath ,O_WRONLY| O_RDWR | O_CREAT | O_TRUNC, S_IRWXU );
										close( fd );
                                    }                                    
                                }
                                
                        	#endif                            
                        }
                    }
                }
                    
				#if 0                
		        info( "---------------------------\n" );
				info( ">> 影片总集数:%d\n", movlist->total_videos );
				info( ">> 影片总大小:%llu\n", movlist->total_movsize );
		        info( ">> 影片编码:%s\n", movlist->movcode );
		        info( "------\n");
                info( "\tfilecode:%s\n", movlist->file_info.filecode );
		        info( "\tfilename:%s\n", movlist->file_info.filename );
		        info( "\tfilepath:%s\n",  movlist->file_info.filepath );
		        info( "\tfilesize:%llu\n", movlist->file_info.filesize );
		        info( "\tfiletype:%d\n", movlist->file_info.filetype );
		        info( "\tfilePF:%d\n",  movlist->file_info.filePF   );
		        info( "------\n");

		        SCAND_VIDEO_INFORMATION_LIST * pvideolist = movlist->videolist;
		        while( pvideolist ){
		            info( "\t------\n" );
					info( "\t\t影片集数:%d\n", pvideolist->number );
		            info( "\t\t视频编码:%s\n",pvideolist->videocode );
					info( "\t\tfilecode:%s\n", pvideolist->file_info.filecode );
			        info( "\t\tfilename:%s\n", pvideolist->file_info.filename );
			        info( "\t\tfilepath:%s\n",  pvideolist->file_info.filepath );
			        info( "\t\tfilesize:%llu\n", pvideolist->file_info.filesize );
			        info( "\t\tfiletype:%ds\n", pvideolist->file_info.filetype );
			        info( "\t\tfilePF:%d\n",  pvideolist->file_info.filePF );
		            info( "\t------\n" );
                    
					pvideolist = pvideolist->next;
		        }

		        info( "---------------------------\n\n" );    
				#endif                
				movlist = movlist->next;
		    }                

            //info( "\t----- end- ----\n\n" );			
			partlist = partlist->next;
        }

        //info( "==============end==================\n\n" );        
        phdlist = phdlist->next;
    }


	return 0;
}
