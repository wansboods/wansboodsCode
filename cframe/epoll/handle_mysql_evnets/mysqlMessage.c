#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "mysql_event.h"
#include "mysqlMessage.h"

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

char * show_message_envent( int type ){
	switch( type ){
		case MM_TEST_SELECT_DATE_FROM_WXXWXX_T:
            return "查询wxxwxx 表中的全部数据";
    	case MM_TEST_DELECT_DATE_FROM_WXXWXX_T:
            return "删除wxxwxx 表中的全部数据";
        case MM_TEST_UPDATE_DATE_FROM_WXXWXX_T:
            return "更新wxxwxx 表中的全部数据";
	    case MM_TEST_INSERT_DATE_FROM_WXXWXX_T:
            return "删除wxxwxx 表中的全部数据";
    }

    return "未知事件";
}

int deal_mbarsys_message_event( int sock, char * ippadr, int type, char * message, int length, void ** msgdata, int * msglen ){
	int retcode = 0;
	info( "处理的消息类型为(%d): %s\n", type, show_message_envent( type ) );
	//消息处理类型
	switch( type ){
		case MM_TEST_SELECT_DATE_FROM_WXXWXX_T:
            retcode = mysql_event_select_date_from_wxxwxx_t( message, length, msgdata, msglen );
            break;
    	case MM_TEST_DELECT_DATE_FROM_WXXWXX_T:
            break;
	    case MM_TEST_UPDATE_DATE_FROM_WXXWXX_T:
            break;            
	    case MM_TEST_INSERT_DATE_FROM_WXXWXX_T:
            break;
		default:
            return -1;
    }
    
    return retcode;
}

