#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <mysql/mysql.h>

#include "tmysql.h"

#define MAX_FIELDNAME_SIZE 128



/*-- 打印日志级别 ----------------------------------------*/
#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn   dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug  dzlog_debug
	#define notice  dzlog_notice
#else
	#define info   printf
	#define warn  printf
	#define error  printf
	#define fatal  printf
	#define debug printf
	#define notice printf
#endif


static char *safestrdupMysql( char * str ){
	if( str )
        return strdup( str );
    return NULL;
}

HORIZONTAL_DETAILS_LIST * create_horizontal_details_node( void ){
    HORIZONTAL_DETAILS_LIST * node;
    if( NULL == ( node = ( HORIZONTAL_DETAILS_LIST *)calloc( sizeof( HORIZONTAL_DETAILS_LIST ), 1 ) ) ){
        
        //printf( "创建节点(%ld)失败:err:%s", sizeof( HORIZONTAL_DETAILS_LIST ),strerror( errno ) );
        return NULL;    
    }
    
    node->next = NULL;
    return node;
}

TABLEBODY_DETAILS * create_tablebody_details_node( void ){
    TABLEBODY_DETAILS * node;
    if( NULL == ( node = ( TABLEBODY_DETAILS *)calloc( sizeof( TABLEBODY_DETAILS ), 1 ) ) ){
        //printf( "创建节点(%ld)失败:err:%s", sizeof( HORIZONTAL_DETAILS_LIST ),strerror( errno ) );
        return NULL;    
    }
    
    node->next = NULL;
    return node;
}

void add_node_into_horizontal_details_list( HORIZONTAL_DETAILS_LIST ** list, HORIZONTAL_DETAILS_LIST * node ){

    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        *list = node;
        return;
    }

    HORIZONTAL_DETAILS_LIST * stepnode = *list;
    while( stepnode ){
        if( !stepnode->next ){
            
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }

    return;
}

void add_node_into_tablebody_details_list( TABLEBODY_DETAILS ** list, TABLEBODY_DETAILS * node ){
    if( NULL == list )
    {
        return;
    }

    if( NULL == *list )
    {
        *list = node;
        return;
    }

    TABLEBODY_DETAILS * stepnode = *list;
    while( stepnode )
    {
        if( !stepnode->next )
        {
            stepnode->next = node;
            return;
        }
        
        stepnode = stepnode->next;
    }

    return;
}

void free_tablebody_details_list( TABLEBODY_DETAILS ** list ){
    if( NULL == list )
    {
        return;
    }

    if( NULL == *list )
    {
        return;
    }

    TABLEBODY_DETAILS * stepnode = *list;
    while( stepnode )
    {
        TABLEBODY_DETAILS * lnext_node = stepnode->next;
		free( stepnode );
		stepnode = lnext_node;         
    }

    return;
}

void free_horizontal_details_list( HORIZONTAL_DETAILS_LIST ** list ){
    if( NULL == list ){
        return;
    }

    if( NULL == *list ){
        return;
    }

    HORIZONTAL_DETAILS_LIST * stepnode = *list;
    while( stepnode ){
        HORIZONTAL_DETAILS_LIST * hnext_node = stepnode->next;
        free_tablebody_details_list( &stepnode->list );
		free( stepnode );
		stepnode = hnext_node;        
    }

    return;    
}

void print_error( MYSQL *conn, const char *title ){
    //fprintf( stderr, "%s:\nError %u (%s)\n", title, mysql_errno( conn ), mysql_error( conn ) );
    warn( "mysql 内部处理出错: %s: Error(%u) (%s)", title, mysql_errno( conn ), mysql_error( conn )  );
}


MYSQL connectionMysql(){

#define IPADDRESS "127.0.0.1"
#define USERNAME "root"
#define PASSWORD "root"
#define DBNAME   "wxx"
#define DBPORT   3306

    MYSQL my_connection;
	mysql_init( &my_connection );
    if( mysql_real_connect(&my_connection, IPADDRESS, USERNAME, PASSWORD, DBNAME, DBPORT, NULL, 0 ) ){
		info( "连接MYSQL 数据库成功!" );        
		return my_connection;
    }

    warn( "连接数据库失败!" );
    return my_connection;   
}


int queryResultsMysql( MYSQL_RES *result,  HORIZONTAL_DETAILS_LIST ** gdata ){     
	if( NULL == result ){
		warn( "未查询到相关记录信息" );
		return 0;
    }

	//char column[MAX_COLUMN_LEN][MAX_COLUMN_LEN]; //存储空间
	MYSQL_ROW sql_row;
    MYSQL_FIELD *fd;    
    info( "查询记录:%lu ",( unsigned long )mysql_num_rows( result ) );
    info( "\n" );
    int i = 0;
    int fieldnum;
    char fieldname[ MAX_FIELDNAME_SIZE ] = {'\0'};    
    //获取列名
    info( "查询字段:( " );
    for( i = 0; fd = mysql_fetch_field(result);i++ ){  
        bzero( fieldname, sizeof( fieldname ) );
        strcpy( fieldname, fd->name );
        info( "%s ",  fieldname );
    }
    
    info( ")\n" );    
    fieldnum = mysql_num_fields( result );
	info( "查询字段名个数:%d ", fieldnum );

#if 0
	//显示列表名
    for( i = 0; i < j; i++ ){
        printf( "%s \t", column[i] );
    }
#endif
    
    printf( "\n" );
    HORIZONTAL_DETAILS_LIST * phorizontal = NULL;
    HORIZONTAL_DETAILS_LIST * head_horizontal = NULL;

    //获取具体的数据
    while( sql_row = mysql_fetch_row( result ) ){
        phorizontal = create_horizontal_details_node();
	    if( phorizontal == NULL ){
	        warn( "failed to create_horizontal_details_node" );
			free_horizontal_details_list( &head_horizontal );
	        return -1;
	    }                

        add_node_into_horizontal_details_list( &head_horizontal, phorizontal );
        TABLEBODY_DETAILS * headtablebody_details = NULL;
        for( i = 0; i < fieldnum; i ++ ){
            TABLEBODY_DETAILS * ptablebody_details = NULL;
			ptablebody_details = create_tablebody_details_node();
            if( NULL == ptablebody_details ){
				warn( "创建tablebody 节点出错!" );
                return -1;
            }

        	add_node_into_tablebody_details_list( &headtablebody_details, ptablebody_details );
        	ptablebody_details->tablebody_id = i;
            ptablebody_details->data = safestrdupMysql( sql_row[i] );                                    
            //printf( "%s\t", sql_row[i] );
        }

        phorizontal->list= headtablebody_details;
        
        //printf("\n");
    }       

	*gdata = head_horizontal;
	return 0;
}

int queryMysql( char * querystatement, HORIZONTAL_DETAILS_LIST ** pdata ){
	if( NULL == querystatement ){
		warn( "数据库查询条件为nul!" );
		return -1;
    }
    
    MYSQL my_connection;
    MYSQL_RES *result;
    MYSQL_ROW sql_row;
    MYSQL_FIELD *fd;
    
	my_connection = connectionMysql();
    if( mysql_query( &my_connection, querystatement ) ){
        //warn( "查询命令,执行结果出错:%s", print_error( my_connection, "连接信息" ) );
        print_error( &my_connection, "连接信息" );
        return -1;
	}

	//保存查询到的数据到result
    result = mysql_store_result( &my_connection );    
    //查询结果
    queryResultsMysql( result, pdata );    
    //释放结果资源
    mysql_free_result( result ); 
    
	//断开连接
    mysql_close( &my_connection );
    return 0;
}

int insert_update_del_Mysql( char * querystatement ){
	if( NULL == querystatement ){
		warn( "数据库查询条件为nul!" );
		return -1;
    }
    
    MYSQL my_connection;
    MYSQL_RES *result;
    MYSQL_ROW sql_row;
    MYSQL_FIELD *fd;
    
	my_connection = connectionMysql();
    if( mysql_query( &my_connection, querystatement ) ){
        //warn( "查询命令,执行结果出错:%s", print_error( my_connection, "连接信息" ) );
        print_error( &my_connection, "连接信息" );
        return -1;
	}
    
	//断开连接
    mysql_close( &my_connection );
    return 0;
}

#ifdef TEST_MAIN
int main( int argc , char *argv[]){

	//insert_update_del_Mysql( "insert into employeeInformation_t value( 4, 'Lion' )" );    
	insert_update_del_Mysql( "update  employeeInformation_t  set  name = 'Lion'  where employee_id = 4" );    
   	HORIZONTAL_DETAILS_LIST * hpdata = NULL;
	queryMysql( "select * from employeeInformation_t", &hpdata );    
    HORIZONTAL_DETAILS_LIST * pdata = hpdata;
    debug( "数据库查询内容: [ \n" );
    while( pdata ){
		TABLEBODY_DETAILS * pist = pdata->list;
		while( pist ){
            if( pist->tablebody_id == 0 )
	            debug( "内容( id )  = %s \n", pist->data );
           	if( pist->tablebody_id == 1 )
	 			debug( "内容(name) = %s \n", pist->data );
            pist = pist->next;
        }

        debug( "------------------\n" );
		pdata = pdata->next;
    }

    debug( "    ]\n" );
    free_horizontal_details_list( &hpdata );    
    return 0;
}
#endif
