#ifndef __MYSQLMESSAGE_H__
#define __MYSQLMESSAGE_H__


//�¼�ö�� xxxxx  
// ��һλ: 1:��ѯ/2:ɾ��/:3�޸�/:4����
// �ڶ�δ: 1:oracle����/2:mysql����
// ���� ~ ��λ: �¼���� 
typedef enum{

	MM_TEST_SELECT_DATE_FROM_WXXWXX_T = 11000,
    MM_TEST_DELECT_DATE_FROM_WXXWXX_T = 21000,
    MM_TEST_UPDATE_DATE_FROM_WXXWXX_T = 31000,
    MM_TEST_INSERT_DATE_FROM_WXXWXX_T  = 41000,

}MM_OPDB_EVENT;


int deal_mbarsys_message_event( int sock, char * ippadr, int type, char * message, int length, void ** msgdata, int * msglen );



#endif //__MYSQLMESSAGE_H__