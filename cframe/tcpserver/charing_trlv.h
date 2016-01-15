#ifndef __CHARING_TRLV_H__
#define __CHARING_TRLV_H__

typedef struct{
    int type;
    int result;
    int length;
}CHARING_TRLV_MESSAGE_HEAD;

typedef struct
{
    char keyword;
    int length;
    char data[ 128 ];
}DISCOVER_TRANSFER_DATA_T;

int charing_data_in_mbarsys_msg_channel( int sock, int msgtype, void * sndmsg, int sndlen, void * * rcvmsg, int * rcvlen, int timeout );
int charing_open_custom_msg_channel( int port, char * netif );
int charing_open_discover_msg_channel( int port );
int charing_close_mbarsys_msg_channel( int sock );
int monitor_charing_msg_channel( int sock );

int send_trlv_data_to_tcp_communication_channel( int sockfd, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void * message, int msglen );
int receive_charing_data_from_tcp_communication_channel( int sockfd, CHARING_TRLV_MESSAGE_HEAD * head, int headlen, void ** message, int timeout );

int send_data_to_discover_msg_channel( int sock, char keyword, char * msgdata, int msglen, char * ipaddr, unsigned short port );
int receive_data_from_discover_msg_channel( int sock, char * keyword, void ** message, int * msglen, char * ipaddr, int length, unsigned short * port, int timeout );

   
#endif //__CHARING_TRLV_H__
