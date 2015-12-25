#ifndef __SCAND_BASICLIB_H__
#define __SCAND_BASICLIB_H__




void scand_safe_free( void * ptr );
int scand_safe_memcmp( char * sstring, char * tstring, int length );
int scand_safe_strcmp(  char * sstring, char * tstring );
int scand_safe_casestrcmp( char * sstring, char * tstring );

// !!! 生成的文件需释放
char *scand_safestrdup( const char * str );
int scand_valid_num_values( char * filename );
int scand_newDirectory( char * dir );

int scand_ExecuteCommand( char * command );

#endif

