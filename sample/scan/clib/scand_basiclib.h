#ifndef __SCAND_BASICLIB_H__
#define __SCAND_BASICLIB_H__




void scand_safe_free( void * ptr );
int scand_safe_memcmp( char * sstring, char * tstring, int length );


// !!! 生成的文件需释放
char *scand_safestrdup( const char * str );


#endif

