#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

#ifdef OPENZLOG
	#define info    dzlog_info	
	#define warn    dzlog_warn
	#define fatal   dzlog_fatal
	#define error   dzlog_error
	#define debug   dzlog_debug
	#define notice  dzlog_notice
#else
	#define info printf
	#define warn printf 
    #define error  printf
	#define fatal  printf
	#define debug  printf
	#define notice printf
#endif


#define ZLOG_BASIC_PATH "/data/d2dcopy/running/config/"
#define ZLOG_CHARING_CATEGROY "CHARING"

int main( int argc, char ** argv ){
    int retcode = 0;
    char zlogcfg[ 512 ] = { '\0' };
    sprintf( zlogcfg, "%s%s", ZLOG_BASIC_PATH, ZLOG_BASIC_PATH );	
    // ��ʼ��zlog
    retcode = dzlog_init( zlogcfg, ZLOG_CHARING_CATEGROY );
    if( retcode ){
	    printf( "[%s][%d]  ��־����ʧ��! ������:%d, ���������ļ�%s �Ƿ���ȷ!\n", __FUNCTION__,__LINE__, retcode, zlogcfg );
    }
    
    // ����ϵͳ��SIGPIPE ��Ĭ�ϴ�����ɱ��������̴���
    signal( SIGPIPE, SIG_IGN );
    info( "������תΪ��̨�ػ�����" );
    goto_daemon( 0, 0 );
    start_charing_manage_discover_monitor_task();
    info( "�����������������" );    
    start_charing_heart();
	info( "���ղ������Ż�������������Ϣ" );
    start_receive_deal_charing_manage_message();
    info( "����hdplayermange ������������Ϣ" );
    start_receive_deal_hdplayer_manage_message();
    
	return 0;   
}
