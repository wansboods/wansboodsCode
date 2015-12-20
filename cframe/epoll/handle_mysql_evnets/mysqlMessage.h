#ifndef __MYSQLMESSAGE_H__
#define __MYSQLMESSAGE_H__

typedef enum{


}


int deal_mbarsys_message_event( int sock, char * ippadr, int type, char * message, int length, void ** msgdata, int * msglen );



#endif //__MYSQLMESSAGE_H__