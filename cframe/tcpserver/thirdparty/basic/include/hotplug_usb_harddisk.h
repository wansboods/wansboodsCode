#ifndef __HOTPLUG_USB_HARDDISK_H
#define __HOTPLUG_USB_HARDDISK_H

#include "systype.h"

typedef enum
{
    STATUS_USBDEV_UNKNOWN,
    STATUS_USBDEV_INSERT,
    STATUS_USBDEV_PLUG
}USBDEV_STATUS_DEFINE;

INT32_T create_hotplug_monitor_socket( VOID );
RTSTATUS monitor_usb_harddisk_hotplug_notify( INT32_T sock, INT8_T * devname, INT32_T length, USBDEV_STATUS_DEFINE * status );
RTSTATUS destroy_hotplug_monitor_socket( INT32_T sock );

#endif//__HOTPLUG_USB_HARDDISK_H
