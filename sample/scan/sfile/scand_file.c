#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

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


char * scand_dofile( char * filename, unsigned long long int * filesize );





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


int reead_content_from_hdsn_file( char * serialnoPath, char *serialno, int length ){
	if( NULL == serialnoPath ){
		warn( "传入的值出错!" );
		return -1;
    }

    int retcode = 0;
    unsigned long long int filesize = 0;    
	char * fileContent = scand_dofile( serialnoPath, &filesize );
    if( NULL == fileContent ){
		warn( "无法获取文件内容,内容为nul!" );
		return -1;
    }
    
    info( "文件(%s) 大小是%llu", serialnoPath, filesize );
    if( filesize > length ){
		warn( "文件内容大于存入的推栈, 推栈空间为%d", length );
        retcode = SCAND_OPFILE_OUT_OF_MEMORY;
    }else{
		memcpy( serialno, fileContent, strlen( fileContent ) + 1 );
    }

    scand_safe_free( fileContent );
    return retcode;
}


