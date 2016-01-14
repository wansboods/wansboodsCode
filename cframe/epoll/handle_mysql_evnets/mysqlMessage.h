#ifndef __MYSQLMESSAGE_H__
#define __MYSQLMESSAGE_H__


//事件枚举 xxxxx  
// 第一位: 1:查询/2:删除/:3修改/:4插入
// 第二未: 1:oracle操作/2:mysql操作
// 第三 ~ 五位: 事件序号 
typedef enum{

	MM_TEST_SELECT_DATE_FROM_WXXWXX_T = 11000,
    MM_TEST_DELECT_DATE_FROM_WXXWXX_T = 21000,
    MM_TEST_UPDATE_DATE_FROM_WXXWXX_T = 31000,
    MM_TEST_INSERT_DATE_FROM_WXXWXX_T  = 41000,

}MM_OPDB_EVENT;


int deal_mbarsys_message_event( int sock, char * ippadr, int type, char * message, int length, void ** msgdata, int * msglen );



#endif //__MYSQLMESSAGE_H__