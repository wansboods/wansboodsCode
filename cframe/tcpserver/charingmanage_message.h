#ifndef __CHARINGMANAGE_MESSAGE_H__
#define __CHARINGMANAGE_MESSAGE_H__


typedef enum{
    CHARINGMANAGE_MSGTYPE_PLAY_CONTROLS  = 50002, //播放控制事件
    CHARINGMANAGE_MSGTYPE_PLAY_CHANGE_STATUS = 50003,  //播放机状态改变事件
    CHARINGMANAGE_MSGTYPE_VOL_CHANGE_STATUS = 50004,   //音量改变事件

	CHARINGMANAGE_MSGTYPE_REGISTER_DEVICE = 50006, //发送心跳包
	CHARINGMANAGE_MSGTYPE_GET_PLAY_STATUS = 50007, //获取播放机状态
	CHARINGMANAGE_MSGTYPE_CHANGE_MUTE =    50008, //静音改变事件
    
}CHARINGMANAGE_MSGTYPE;

typedef int ( * CHARINGMANAGE_MESSAGE_DEAL_CB )( int sock, char * ipaddr, int msgtype, void *rcvmsgdata,int rcvmsglen, void ** replymsgdata, int * replymsglen );

int start_receive_deal_hdplayer_manage_message();


#endif //__CHARINGMANAGE_MESSAGE_H__
