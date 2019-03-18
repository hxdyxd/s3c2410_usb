
/*
 * (C) Copyright 2001
 * Denis Peter, MPL AG Switzerland
 *
 * Most of this source has been derived from the Linux USB
 * project.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 *
 */
//#include <string.h>
//#include "ctype.h"
#include "usb.h"
#include "s3c2410.h"

//by hxdyxd
#include "app_debug.h"


static int usb_stor_curr_dev=-1; /* current device */
/* some display routines (info command) */
unsigned long simple_strtoul(const char *cp,char **endp,unsigned int base)
{
    unsigned long result = 0,value;
    if (*cp == '0') 
    {
        cp++;
        if ((*cp == 'x') && isxdigit(cp[1])) 
        {
            base = 16;
            cp++;
        }
        if (!base) 
        {
            base = 8;
        }
    }
    if (!base) 
    {
        base = 10;
    }
    while (isxdigit(*cp) && (value = isdigit(*cp) ? *cp-'0' : (islower(*cp)
    ? toupper(*cp) : *cp)-'A'+10) < base) 
    {
        result = result*base + value;
        cp++;
    }
    if (endp)
    *endp = (char *)cp;
    return result;
}


/*
"reset - reset (rescan) USB controller\r\n"
"usb read addr blk# cnt - read `cnt' blocks starting at block `blk#'\r\n"
"    to memory address `addr'\r\n"
*/
int s_usbhost_reset(void)
{
    int i;
    // struct usb_device *dev = NULL;
    // block_dev_desc_t *stor_dev;
     
    usb_stop();
    APP_DEBUG("[%d] Reset USB...\r\n", __LINE__);
    i = usb_init();
    /* try to recognize storage devices immediately */
    if (i >= 0) {
        usb_stor_curr_dev = usb_stor_scan(1);
    }
    if(usb_stor_curr_dev < 0) {
        return -1;
    }
    return 0;
}


int s_usbhost_start(void)
{
    int i;
    //struct usb_device *dev = NULL;
    //block_dev_desc_t *stor_dev;
     
    //usb_stop();
    
    s_UartPrint("(Re)start USB...\r\n");
    i = usb_init_22();
    /* try to recognize storage devices immediately */
    if (i >= 0) {
        usb_stor_curr_dev = usb_stor_scan(1);
    }
    if(usb_stor_curr_dev < 0) {
        APP_WARN("\033[40;31m No USB Storage Device(s) found!!! \033[0m\r\n");
        return -1;
    }
    return 0;
}



/*display the usb status, storage device message if there is have one*/
/*
 *set current device
 */
int s_usbhost_dev(int dev)
{
    block_dev_desc_t *stor_dev;

    if (dev >= USB_MAX_STOR_DEV) {
        APP_WARN("unknown device\r\n");
        return 1;
    }
    stor_dev = usb_stor_get_dev(dev);
     
    if (stor_dev->type == DEV_TYPE_UNKNOWN) {
        APP_WARN("unknown device type\r\n");
        return 1;
    }
    usb_stor_curr_dev = dev;
    return 0;
}


int s_usbhost_read(int dev, unsigned long rblk,unsigned long rcnt,unsigned char* rbuffer)
{
    unsigned long addr =(unsigned long)rbuffer;
    unsigned long blk  = rblk;
    unsigned long cnt  = rcnt;
    unsigned long have_read;

    if(s_usbhost_dev(dev) == 1) {
        return -1;
    }

    APP_DEBUG("USB read: device %d block # %ld, count %ld  addr %08x... \r\n",\
    dev, blk, cnt, addr);
    have_read = usb_stor_read(dev, blk, cnt, (ulong *)addr);
     
    if (have_read != cnt) {
        APP_ERROR("%ld blocks read: ERROR\r\n", have_read);
        return -1;
    }
    APP_DEBUG("%ld blocks read: OK\r\n", have_read);
    return have_read;
}


/* end of file*/