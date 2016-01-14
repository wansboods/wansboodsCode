#define _LARGEFILE_SOURCE
#define _LARGEFILE64_SOURCE
#define _FILE_OFFSET_BITS 64

#include <time.h>
#include <ctype.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>

#include <scsi/sg.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/statfs.h>
#include <sys/types.h>
#include <linux/hdreg.h>

#include "scand_file.h"
#include "scand_basiclib.h"

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

#define SCAND_2_DIR  10 //向下扫描两级目录  +4
#define SCAND_3_DIR  14 //向下扫描三级目录  +4
#define SCAND_4_DIR  18 //向下扫描四级目录  +4


#define MAX_SEARCHPATH_LENHTH  2048
#define SCAND_MBAR_MOVIS_PATH  "/mbar/movies"


int fileparsing( char *suffix, SCAND_FILE_TYPE *filetype, SCAND_PACKAGE_FORMAT * filePF );

char * scand_dofile( char * filename, unsigned long long int * filesize );
void directoryTraversal( SCAND_MOVIE_INFORMATION_LIST ** filelist, char *path, int indent );

SCAND_MOVIE_INFORMATION_LIST * create_movieinfo_node();
SCAND_VIDEO_INFORMATION_LIST * create_videoinfo_node();

void add_node_into_movie_list( SCAND_MOVIE_INFORMATION_LIST ** list, SCAND_MOVIE_INFORMATION_LIST * node );
void add_node_into_video_list( SCAND_VIDEO_INFORMATION_LIST ** list, SCAND_VIDEO_INFORMATION_LIST * node );

void free_movie_list( SCAND_MOVIE_INFORMATION_LIST ** list );
void free_video_list( SCAND_VIDEO_INFORMATION_LIST ** list );

int fill_movie_node( char * filepath, SCAND_FILE_TYPE filetype, SCAND_MOVIE_INFORMATION_LIST * movnode );
int fill_video_node( char * path, SCAND_VIDEO_INFORMATION_LIST * video_info );

int search_movie_info_from_mbarhdisk( char *path, SCAND_MOVIE_INFORMATION_LIST ** filelist );



char * show_SCAND_PACKAGE_FORMAT( SCAND_PACKAGE_FORMAT status ){
	switch( status ){
		case SCAND_PF_ISO:
            return "ISO 文件";
        case SCAND_PF_MKV:
            return "MKV 文件";
        case SCAND_PF_TS:
            return "TS 文件";
        case SCAND_PF_MP4:
            return "MP4 文件";
        case SCAND_PF_FLV:
            return "FLV 文件";
        case SCAND_PF_MP3:
            return "MP3 文件";
        case SCAND_PF_APE:
            return "APE 文件";
        case SCAND_PF_JPEG:
            return "JPEG 文件";
        case SCAND_PF_GIF:    
            return "GIF 文件";
    }

    return "未知";
}

char * show_SCAND_FILE_TYPE( SCAND_FILE_TYPE status ){

	switch( status ){
		case SCAND_OTHER_FILE:
            return "其他文件";
        case SCAND_VIDEO_FILE:
            return "视频文件";
        case SCAND_AUDIO_FILE:
            return "音频文件";
        case SCAND_PICTURE_FILE:
            return "图片文件";
        case SCAND_DIR_FILE:
            return "目录文件";
        case SCAND_LINK_FILE:
            return "链接文件";

    }

    return "未知";
}



char * scand_dofile( char * filename, unsigned long long int * filesize ){
	if( NULL == filename ){
		warn( "传入的参数出错!" );
        return NULL;
    }
    
	FILE * f = fopen( filename, "rb" );
    fseek( f, 0, SEEK_END );
    *filesize = ftell( f );
    
    fseek( f, 0, SEEK_SET );
	char *data = ( char *)calloc( *filesize + 1, 1 );
    fread( data, 1, *filesize, f );
    fclose( f );

    return data;    
}


int read_content_from_hdsn_file( char * serialnoPath, char *serialno, int length ){
	if( NULL == serialnoPath ){
		warn( "传入的值出错!" );
		return -1;
    }

    int retcode = 0;
    unsigned long long int filesize = 0;    
	char * fileContent = scand_dofile( serialnoPath, &filesize );
    if( NULL == fileContent ){
		warn( "无法获取文件内容,内容为nul!\n" );
		return -1;
    }
    
    info( "文件(%s) 大小是%llu\n", serialnoPath, filesize );
    if( filesize > length ){
		warn( "文件内容大于存入的推栈, 推栈空间为%d\n", length );
        retcode = SCAND_OPFILE_OUT_OF_MEMORY;
    }else{
		memcpy( serialno, fileContent, strlen( fileContent ) + 1 );
    }

    scand_safe_free( fileContent );
    return retcode;
}


void directoryTraversal( SCAND_MOVIE_INFORMATION_LIST ** filelist, char *path, int indent ){
    DIR *pdir;
    struct stat statbuf;
    struct dirent* ent = NULL;  
    if( NULL == ( pdir = opendir( path ) ) ){
		warn( "打开文件(%s ) 失败, err:%s\n", path, strerror( errno ) );             
		return;
    }

    while( ( ent = readdir( pdir ) ) != NULL ){
		//排除当前目录
		if( ( scand_safe_strcmp( ".", ent->d_name ) ) == 0  ){
			continue;
        }

        //排除上级目录
		if( ( scand_safe_strcmp( "..", ent->d_name ) ) == 0  ){
			continue;
        }

        //得到读取文件的绝对路径名
	    char dir[ MAX_SEARCHPATH_LENHTH ]={ '\0' };        
        snprintf( dir, MAX_SEARCHPATH_LENHTH, "%s/%s", path, ent->d_name );
       	//printf( ":: dir:%s\n", dir );
#if 1        
        //得到文件信息   
        lstat( dir, &statbuf );       
        //判断是目录还是文件  
        if( S_ISDIR( statbuf.st_mode ) ){

            if( SCAND_2_DIR == indent ){
                //printf( ":: break!\n" );
				break;
            }

            //printf( "indent=%d\n", indent );
            //printf( "%*s子目录:%s/\n", indent, "", ent->d_name );
            search_movie_info_from_mbarhdisk( dir, filelist );
            directoryTraversal( filelist, dir, indent + 4 ); 
        }else{
            search_movie_info_from_mbarhdisk( dir, filelist );
            //directoryTraversal( filelist, dir, indent + 4 ); 
        }      
#endif        
    }

    closedir( pdir );    
}

void show_movie_file( SCAND_MOVIE_INFORMATION_LIST * movlist ){
	SCAND_FILE_INFO file_info;      
    SCAND_VIDEO_INFORMATION_LIST * videolist;
    
	while( movlist ){
        info( "---------------------------\n" );
		info( ">> 影片总集数:%d\n", movlist->total_videos );
		info( ">> 影片总大小:%llu\n", movlist->total_movsize );
        info( ">> 影片编码:%s\n", movlist->movcode );
        info( "------\n");
		info( "\tfilecode:%s\n", movlist->file_info.filecode );
        info( "\tfilename:%s\n", movlist->file_info.filename );
        info( "\tfilepath:%s\n",  movlist->file_info.filepath );
        info( "\tfilesize:%llu\n", movlist->file_info.filesize );
        info( "\tfiletype:%d %s\n", movlist->file_info.filetype, show_SCAND_FILE_TYPE( movlist->file_info.filetype ) );
        info( "\tfilePF:%d %s\n",  movlist->file_info.filePF, show_SCAND_PACKAGE_FORMAT( movlist->file_info.filePF ) );
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
	        info( "\t\tfiletype:%d %s\n", pvideolist->file_info.filetype, show_SCAND_FILE_TYPE( pvideolist->file_info.filetype )  );
	        info( "\t\tfilePF:%d %s\n",  pvideolist->file_info.filePF, show_SCAND_PACKAGE_FORMAT( pvideolist->file_info.filePF ) );
            info( "\t------\n" );
			pvideolist = pvideolist->next;
        }

        info( "---------------------------\n\n" );            
		movlist = movlist->next;
    }
}
    
int read_movie_file_from_mbar_hddisk( SCAND_HARDDISK_STYLE style, char *path, SCAND_MOVIE_INFORMATION_LIST ** movlist ){
	if( NULL == path ){
		warn( "传入值出错!" );
        return -1;
    }

    int retcode = 0;
	char searchpath[ MAX_SEARCHPATH_LENHTH ] = { '\0' };
    switch( style ){
		case EM_SOURCE_HD:
            memcpy( searchpath, path, strlen( path ) + 1 );            
			info( ">>>>>>>> searchpath:%s\n", searchpath );
            directoryTraversal( movlist, searchpath, 2 );                
            break;
        case EM_TARGET_HD:
			sprintf( searchpath, "%s%s", path, SCAND_MBAR_MOVIS_PATH );
            if( access( searchpath, R_OK ) != 0 ) {
				memcpy( searchpath, path, strlen( path ) + 1 );
            }
            
			info( ">>>>>>>> searchpath:%s\n", searchpath );
            directoryTraversal( movlist, searchpath, 2 );
            
            break;
    }


	//SCAND_MOVIE_INFORMATION_LIST * pmovlist = *movlist;
	//show_movie_file( pmovlist );
    return retcode;
}

    
int search_movie_info_from_mbarhdisk( char * path, SCAND_MOVIE_INFORMATION_LIST ** filelist ){
	if( NULL == path ){
		warn( "传入值出错!" );
        return -1;
    }
    
    //info( "::path:%s\n", path );
	int retcode = 0;
    SCAND_FILE_TYPE movfiletype;
    unsigned long long int filesize = 0;
    char filepath[ MAX_SEARCHPATH_LENHTH ] = { '\0' };
    char jumpfilepath[ MAX_SEARCHPATH_LENHTH ] = { '\0' };
        
    if( strlen( path ) + 1 > sizeof( filepath ) ){
		warn( "文件内容大于存入的推栈, 推栈空间为%ld\n", sizeof( filepath ) );
		return -1;
    }
    
	memcpy( filepath, path, strlen( path ) + 1 );
    //warn( ":: filepath:%s\n", filepath );
	struct stat64 status;
	if( lstat64( filepath, &status ) != 0 ){
		warn( "使用lstat64 函数出错!\n" );
	    return -1;
	}

    if( S_ISLNK( status.st_mode ) ){
		warn( "文件(%s) 是一个链接文件\n", filepath );
		movfiletype = SCAND_LINK_FILE;       
    }else if( S_ISDIR( status.st_mode ) ){
		warn( "文件(%s) 是一个目录文件\n", filepath );
		return 0;
    }else if( S_ISCHR( status.st_mode ) ){
		warn( "文件(%s) 是一个字符设备文件\n", filepath );		
		return 0;
	}else if( S_ISBLK( status.st_mode ) ){
		warn( "文件(%s) 是一个块设备文件\n", filepath );		
		return 0;
    }else{
  		warn( "文件(%s) 是一个其他文件\n", filepath );		
		movfiletype = SCAND_OTHER_FILE;        
    }
    
    //warn( "---- movfiletype:%d\n", movfiletype );
    SCAND_MOVIE_INFORMATION_LIST * movnode = create_movieinfo_node();
    if( NULL == movnode ){
		warn( "创建SCAND_MOVIE_INFORMATION_LIST 失败" );
		return -1;
    }

    SCAND_VIDEO_INFORMATION_LIST * video_node = NULL;
    if( movfiletype == SCAND_LINK_FILE ){
		ssize_t rslt = 0;
		char linkpath[ MAX_SEARCHPATH_LENHTH ] = { '\0' };		
        rslt = readlink( filepath, linkpath, sizeof( linkpath ));
        if( rslt < 0 || retcode >= sizeof( filepath ) ){
			warn( "使用readlink 函数出错, 读取到的内容大小是%ld", rslt );
            return -1;
        }

        linkpath[ rslt ] = '\0';
		sprintf( jumpfilepath, "%s/%s", path, linkpath );
        video_node = create_videoinfo_node();
        if( NULL == video_node ){
			warn( "创建SCAND_VIDEO_INFORMATION_LIST 失败" );
			return -1;
        }

        retcode = fill_video_node( jumpfilepath, video_node );
		if( retcode ){
			warn( "[%s][%d] ( %s )解析出错!\n", __FUNCTION__, __LINE__, jumpfilepath );
            return -1;
        }
    }else if( movfiletype == SCAND_OTHER_FILE ){

	    video_node = create_videoinfo_node();
	    if( NULL == video_node ){
			warn( "创建SCAND_VIDEO_INFORMATION_LIST 失败" );
			return -1;
	    }

	    retcode = fill_video_node( filepath, video_node );
		if( retcode ){
			warn( "[%s][%d] ( %s )解析出错!\n", __FUNCTION__, __LINE__, filepath );
	        return -1;
	    }
    }else{

    }
    
    add_node_into_video_list( &movnode->videolist, video_node );            
    add_node_into_movie_list( filelist, movnode );    
	fill_movie_node( filepath, movfiletype, movnode );
    return 0;
}


SCAND_MOVIE_INFORMATION_LIST * create_movieinfo_node(){
    SCAND_MOVIE_INFORMATION_LIST * node;
    if( NULL == ( node = ( SCAND_MOVIE_INFORMATION_LIST * )calloc( sizeof( SCAND_MOVIE_INFORMATION_LIST ), 1 ) ) ){
        return NULL;
    }

    return node;
}


SCAND_VIDEO_INFORMATION_LIST * create_videoinfo_node(){
    SCAND_VIDEO_INFORMATION_LIST * node;
    if( NULL == ( node = ( SCAND_VIDEO_INFORMATION_LIST * )calloc( sizeof( SCAND_VIDEO_INFORMATION_LIST ), 1 ) ) ){
        return NULL;
    }

    return node;
}

void add_node_into_movie_list( SCAND_MOVIE_INFORMATION_LIST ** list, SCAND_MOVIE_INFORMATION_LIST * node ){
    if( NULL == list ){
        return;
    }
    
    if( NULL == *list ){
        *list = node;
        return;
    }
    
    SCAND_MOVIE_INFORMATION_LIST * stepnode = *list;
    while( stepnode )
    {
        if( !stepnode->next )
        {
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }
}

void add_node_into_video_list( SCAND_VIDEO_INFORMATION_LIST ** list, SCAND_VIDEO_INFORMATION_LIST * node ){
    if( NULL == list ){
        return;
    }
    
    if( NULL == *list ){
        *list = node;
        return;
    }
    
    SCAND_VIDEO_INFORMATION_LIST * stepnode = *list;
    while( stepnode ){
        if( !stepnode->next ){
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }
}    


void free_movie_list( SCAND_MOVIE_INFORMATION_LIST ** list ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    SCAND_MOVIE_INFORMATION_LIST * stepnode = *list;
    while( stepnode ){
        SCAND_MOVIE_INFORMATION_LIST * nextnode = stepnode->next;
        free_video_list( &stepnode->videolist );
        free( stepnode );
        stepnode = nextnode;
    }
    
    *list = NULL;
}



void free_video_list( SCAND_VIDEO_INFORMATION_LIST ** list ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    SCAND_VIDEO_INFORMATION_LIST * stepnode = *list;
    while( stepnode ){
        SCAND_VIDEO_INFORMATION_LIST * nextnode = stepnode->next;
        free( stepnode );
        stepnode = nextnode;
    }
    
    *list = NULL;
}


int fill_movie_node( char * filepath, SCAND_FILE_TYPE filetype, SCAND_MOVIE_INFORMATION_LIST * movnode ){

#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif

	char name[ 128 ] = { '\0' };
	char *suffix = NULL;
	char *pname = filepath;
	if( ( suffix = strrchr( pname, '/' ) ) ){
        suffix ++;
		memcpy( name, suffix, strlen( suffix  ) );
        //info( "name:%s\n", name );
    }
    
	int total_videos = 0;
	unsigned long long int total_size = 0;        
	SCAND_VIDEO_INFORMATION_LIST *pvideo_list = movnode->videolist;
	while( pvideo_list ){
		total_videos ++;
        total_size += pvideo_list->file_info.filesize;        

        pvideo_list = pvideo_list->next;
    }

    movnode->total_videos = total_videos;
    movnode->total_movsize = total_size;

    if( NULL == movnode->file_info.filename ){
		movnode->file_info.filename = scand_safestrdup( name );
    }    
        
    if( filetype == SCAND_LINK_FILE ){
		movnode->file_info.filetype = filetype;
        movnode->file_info.filePF = SCAND_PF_UNKNOWN;
        if( NULL == movnode->file_info.filepath ){
	        movnode->file_info.filepath = scand_safestrdup( filepath );
        }

		struct stat64 status;
		if( lstat64( filepath, &status ) != 0 ){
			warn( "使用lstat64 函数出错!\n" );
		    return -1;
		}
        
        movnode->file_info.filesize = status.st_size;
    }else if( ( filetype != SCAND_LINK_FILE ) || ( filetype != SCAND_DIR_FILE ) ){
		movnode->file_info.filetype = movnode->videolist->file_info.filetype;
        movnode->file_info.filePF = movnode->videolist->file_info.filePF;
        movnode->file_info.filesize = movnode->videolist->file_info.filesize;
        
		if( NULL ==  movnode->videolist->file_info.filecode )
	        movnode->file_info.filecode = scand_safestrdup( movnode->videolist->file_info.filecode );
		if( NULL == movnode->file_info.filepath )
	        movnode->file_info.filepath = scand_safestrdup( filepath );        
    }

#ifdef MUTEX
	pthread_mutex_unlock( &mutex );
#endif  

	return 0;
}

int fill_video_node( char * path, SCAND_VIDEO_INFORMATION_LIST * video_info ){
	if( NULL == path ){
		warn( "传入的值出错!" );
        return -1;
    }
    
    int retcode = 0;
    char *ppath = path;
    char suffix[ 128 ] = { '\0' };
	char fullname[ 1024 ] = { '\0' };
	char * pcname = NULL;
    char * psuffix = NULL;
    unsigned long long int filesize = 0;

    struct stat64 videostatus;
    if( lstat64( path, &videostatus ) != 0 ){
		warn( "使用lstat64 函数出错(%s)!", path );
        return -1;
    }
	            
    filesize = videostatus.st_size;    
	if( ( pcname = strrchr( path, '/' ) ) ){
		pcname ++;
        memcpy( fullname, pcname, strlen( pcname ) + 1 );        
        if( ( psuffix = strrchr( pcname, '.' ) ) ){
			psuffix ++;
            memcpy( suffix, psuffix, strlen( psuffix ) + 1 );
        }
    }

	//warn( "suffix:%s pcname:%s \n", suffix, pcname );    
    SCAND_FILE_TYPE filetype;      //文件类型
    SCAND_PACKAGE_FORMAT filePF;    //文件封装格式 
	retcode = fileparsing( suffix, &filetype, &filePF );
    
	int number = 0;                //视频集数
    char filename[ 128 ] = { '\0' };  //视频名称
    char filecode[ 128 ] = { '\0' };   //视频编码
    char * name = NULL;
    if( !( name = strrchr( pcname, '_' ) ) ){

        if( ( psuffix = strrchr( pcname, '.' ) ) ){
	        memcpy( filename, pcname, psuffix - pcname );
            retcode = scand_valid_num_values( filename );
	        if( retcode == 0 ){
				memcpy( filecode, filename, strlen( filename ) + 1 );
	        }
        }else{
			warn( "(%s) 未找到后缀'.' ", ppath );
        }
            
    	number = 1;
    }else{
   		psuffix --;  
        if( ( psuffix - name ) > sizeof( filename ) ){            
            warn( "[%s][%d]:buffer size %ld less than data size %ld\n", __FUNCTION__, __LINE__, sizeof( filename ),  psuffix - pcname );
            return -1;
        }
        
        memcpy( filename, name, psuffix - name );            
        retcode = scand_valid_num_values( filename );
        if( retcode == 0 ){
			memcpy( filecode, filename, strlen( filename ) + 1 );
        }
        
        pcname ++;
        char seqno[ 12 ] = { '\0' };
		if( ( psuffix = strrchr( ppath, '.' ) ) ){
			if( ( psuffix > pcname ) && ( ( psuffix - pcname ) <= sizeof( seqno) ) ){
				psuffix --;
				memcpy( seqno, pcname, psuffix - pcname );
		        retcode = scand_valid_num_values( filename );
		        if( retcode == 0 ){
		            //sscanf( seqno, "%d", number );
                    number = atoi( seqno );
		        }                
            }
        }       
    }

#ifdef MUTEX
	pthread_mutex_lock( &mutex );
#endif       
	video_info->number = number;
    video_info->videocode = scand_safestrdup( filecode );
        
    video_info->file_info.filename = scand_safestrdup( filename );
    video_info->file_info.filecode = scand_safestrdup( filecode );
    video_info->file_info.filepath = scand_safestrdup( path );
    video_info->file_info.filesize = filesize;
    //info( ">>> path:%s filesize:%llu\n", path, filesize );    
    video_info->file_info.filetype = filetype;
    video_info->file_info.filePF   = filePF;

#ifdef MUTEX
	pthread_mutex_unlock( &mutex );
#endif  
	return 0;
}


#define FILE_SUFFIX_TS "ts"
#define FILE_SUFFIX_ISO "iso"
#define FILE_SUFFIX_MKV "mkv"
#define FILE_SUFFIX_MP4 "mp4"
#define FILE_SUFFIX_BD_ISO "bd.iso"
#define FILE_SUFFIX_3D_ISO "3d.iso"
#define FILE_SUFFIX_3D3D_ISO "3d.3d.iso"


int fileparsing( char *suffix, SCAND_FILE_TYPE *filetype, SCAND_PACKAGE_FORMAT * filePF ){
	if( NULL == suffix ){
		warn( "传入值出错!\n" );
        return -1;
    }

    if( scand_safe_casestrcmp( suffix, "iso" ) == 0 ){        
        *filePF   = SCAND_PF_ISO;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "3d.iso" ) == 0 ){
        *filePF   = SCAND_PF_ISO;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "mkv" ) == 0 ){
        *filePF   = SCAND_PF_MKV;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "mp4" ) == 0 ){
        *filePF   = SCAND_PF_MP4;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "ts" ) == 0 ){
        *filePF   = SCAND_PF_TS;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "flv" ) == 0 ){
        *filePF   = SCAND_PF_FLV;
        *filetype = SCAND_VIDEO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "mp3" ) == 0  ){
        *filePF   = SCAND_PF_MP3;
        *filetype = SCAND_AUDIO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "ape" ) == 0  ){
        *filePF   = SCAND_PF_APE;
        *filetype = SCAND_AUDIO_FILE;
    }else if( scand_safe_casestrcmp( suffix, "png" ) == 0  ){
        *filePF   = SCAND_PF_PNG;
        *filetype = SCAND_PICTURE_FILE;
    }else if( scand_safe_casestrcmp( suffix, "jpg" ) == 0  ){
        *filePF   = SCAND_PF_JPEG;
        *filetype = SCAND_PICTURE_FILE;
    }
    
    return 0;   
}



int get_path_and_suffix_form_filepath( char * filepath, char * path, int pathlength , char * suffix,int suffixlength  ){
	if( NULL == filepath ){
        warn( "传入值出错!\n" );
		return -1;
    }
    
	char * start_filepath = filepath;
    char * pfilepath = filepath;
        
    if( ( pfilepath = strrchr( filepath, '/' ) ) ){
        if( pfilepath - start_filepath < pathlength ){
			memcpy( path, start_filepath, pfilepath - start_filepath );
        }
        else{
            warn( "传入的buf 值过小, 不满足存放值需要的空间( %ld )\n",  pfilepath - start_filepath );            
            return -1;
        }

        if( ( pfilepath = strrchr( filepath, '.' ) ) ){
	        if( strlen( pfilepath ) + 1 < pathlength ){
				memcpy( suffix, pfilepath, strlen( pfilepath ) + 1 );
	        }
	        else{
            	warn( "传入的buf 值过小, 不满足存放值需要的空间( %ld )\n", strlen( pfilepath ) + 1 );
	            return -1;
	        }
            
        }else{
        	warn( "( %s )未找到后缀名..\n", pfilepath );
			return -1;
        }
        
        return 0;
    }
    
	return -1;
}


#define SCAND_EXPORTS_PATH "/etc/exports"
int wirte_NFSfile( char * local_mount_dir ){
	if( NULL == local_mount_dir ){

		return -1;
    }

	int setflg = 0;
    FILE * fp;
    fp = fopen( SCAND_EXPORTS_PATH, "r" ) ;             
    if( NULL == fp ){
		warn( "打开文件( %s ) 出错! err:%s\n", SCAND_EXPORTS_PATH, strerror( errno )  );
        return -1;
    }
    

    char rline[ 1024 ] = { '\0' };
 	while( fgets( rline, sizeof( rline ), fp ) ){
		if( ( strstr( rline, local_mount_dir ) ) ){
            setflg = 1;
			break;
        }
    }

    fclose( fp );
    if( setflg ){
        info( "%s 已经存在挂载路径( %s )\n", local_mount_dir );        
		return 0;
    }

    fp = fopen( SCAND_EXPORTS_PATH, 'at+' ); 
    if( NULL == fp ){
		warn( "打开文件( %s ) 出错! err:%s\n", SCAND_EXPORTS_PATH, strerror( errno )  );
        return -1;
    }

    char command[ 512 ] = { '\0' };
    sprintf( command, "%s *(rw,sync,no_root_squash,no_subtree_check)\n", local_mount_dir );    
    fwrite( command, strlen( command ) + 1, 1, fp );
    fclose( fp );
    return 0;
}
