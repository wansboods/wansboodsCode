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
    // 初始化zlog
    retcode = dzlog_init( zlogcfg, ZLOG_CHARING_CATEGROY );
    if( retcode ){
	    printf( "[%s][%d]  日志启动失败! 错误码:%d, 请检查配置文件%s 是否正确!\n", __FUNCTION__,__LINE__, retcode, zlogcfg );
    }
    
    // 忽略系统对SIGPIPE 的默认处理是杀死这个进程处理
    signal( SIGPIPE, SIG_IGN );
    info( "将进程转为后台守护进程" );
    goto_daemon( 0, 0 );
    start_charing_manage_discover_monitor_task();
    info( "启动心跳包监控任务" );    
    start_charing_heart();
	info( "接收并处理播放机发来的请求消息" );
    start_receive_deal_charing_manage_message();
    info( "接收hdplayermange 服务器请求消息" );
    start_receive_deal_hdplayer_manage_message();
    
	return 0;   
}
