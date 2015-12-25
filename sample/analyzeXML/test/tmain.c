#include <stdio.h>
#include "xmlstringparse.h"


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

int main(){
    char * msgdata = "<request><username>wangfei</username><crypt_pwd>7b8f36a14fb896111f1eb7bdff8fef10</crypt_pwd></request>";
	// 读取数据
	xmlDocPtr doc = get_doc_object_from_string( msgdata );
    if( NULL == doc ){
        warn( "读取数据(%s) 出错!", msgdata );
        return -1;
    }
    
    xmlNodePtr rootobj = get_root_object_from_doc_object( doc );
    if( NULL == rootobj ){
        warn( "获取根节点出错!" );
        free_doc_object( doc );
        return -1;
    }

    int retcode = 0;
    char username[ 128 ] = {'\0'};
    char password[ 64 ] = {'\0'};
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "username", 0, username, sizeof( username )  );
    retcode = get_value_from_next_layer_xml_object_by_tagname( rootobj, "crypt_pwd", 0, password, sizeof( password ) );

    info( "username:%s,password(%s)\n", username, password );
    free_doc_object( doc );
    return 0;   
}
