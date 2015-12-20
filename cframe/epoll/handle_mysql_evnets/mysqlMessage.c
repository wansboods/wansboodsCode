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
		case 

    }

    return "";
}

int deal_mbarsys_message_event( int sock, char * ippadr, int type, char * message, int length, void ** msgdata, int * msglen ){

	info( "处理的消息类型为(%d): %s", type, show_message_envent( type ) );
	//消息处理类型
	switch(){


    }




    return 0;
}

