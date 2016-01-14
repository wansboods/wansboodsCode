#ifndef __HOTPLUG_NIC_H
#define __HOTPLUG_NIC_H

#include "systype.h"

typedef enum
{
    STATUS_NIC_LINK_UNKNOWN,
    STATUS_NIC_LINK_UP,
    STATUS_NIC_LINK_DOWN,
}NIC_LINK_STATUS_DEFINE;

INT32_T create_hotplug_monitor_nic_socket( VOID );
RTSTATUS monitor_nic_hotplug_notify( INT32_T sock, INT8_T * devname, INT32_T length, NIC_LINK_STATUS_DEFINE * status );
RTSTATUS destroy_hotplug_monitor_nic_socket( INT32_T sock );

#endif//__HOTPLUG_NIC_H
