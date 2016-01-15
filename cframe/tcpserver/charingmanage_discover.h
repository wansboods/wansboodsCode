#ifndef __CHARINGMANAGE_DISCOVER_H
#define __CHARINGMANAGE_DISCOVER_H



typedef struct session{
	int sockfd;
	int count;
    
	char ipaddr[ 64 ];
	char hostname[ 128 ];    

    struct session *next;
}HEART_INFO_T; 

int lock( void );
int unlock( void );

int start_charing_heart( void );
int start_charing_manage_discover_monitor_task();

HEART_INFO_T * create_heart_information_list_node_ext( int fd, char * hostname, char * ipaddr );
void add_node_into_heart_information_list( HEART_INFO_T ** list, HEART_INFO_T * node );

int update_register_information_in_heart_information_node( HEART_INFO_T * node, int fd, char * hostname, char * ipaddr );
int registration_hostname(  int sock, char * hostname, char * ipaddr );

#endif //__CHARINGMANAGE_DISCOVER_H
