#ifdef __MBARMID_API_H__
#define __MBARMID_API_H__



RTSTATUS get_box_IP( INT8_T * uuid, INT8_T * gip, INT32_T length );
RTSTATUS search_movinfo( INT32_T kind, INT8_T *keys, INT32_T pagenum, INT32_T pagesize,  INT8_T ** basicxml, INT8_T ** movxml );
RTSTATUS mbarmidapi_get_box_running_status( INT8_T * uuid, INT8_T ** boxxml, INT8_T ** movxml );

//RTSTATUS video_on_demand( INT8_T * uuid, INT32_T kind, INT32_T vid, UINT32_T offset );
RTSTATUS pause_film( INT32_T kind, INT8_T * uuid );
RTSTATUS stop_film( INT32_T kind, INT8_T * uuid );
RTSTATUS mute_film( INT32_T kind, INT8_T * uuid, INT32_T value );
RTSTATUS change_volume_film( INT32_T kind, INT8_T * uuid, INT32_T value );
RTSTATUS shutdown_box( INT32_T kind, INT8_T * uuid );
RTSTATUS get_status_from_playbox( INT8_T * uuid );


//RTSTATUS video_on_demand( INT8_T * uuid, INT32_T kind, INT32_T mid, INT32_T vid, INT32_T timeoff, INT32_T dimension, INT32_T audioidx, INT32_T subidx  );

RTSTATUS search_movinfo_from_order_list( INT8_T * group, INT8_T * uuid, INT32_T rid );
RTSTATUS video_code_info( INT8_T * ptdev, INT8_T * video_rpath );



#endif //__MBARMID_API_H__
