#ifndef   _CMD_USB_H_
#define   _CMD_USB_H_

#include "usb.h"
#include "s3c2410.h"

int s_usbhost_reset(void);
int get_usbhost_status(void);
int s_usbhost_stop(void);
int s_usbhost_tree(void);
int s_usbhost_scan(void);
int s_usbhost_stor(void);
int s_usbhost_info(int argc, char **argv);
int s_usbhost_start(void);
int s_usbhost_read(int dev, unsigned long rblk,unsigned long rcnt,unsigned char* rbuffer);
int usb_scan_devices(void);

#endif /* _CMD_USB_H_ */
