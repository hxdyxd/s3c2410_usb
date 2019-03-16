
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


char * usb_get_class_desc(unsigned char dclass)
{
    switch(dclass) 
    {
    case USB_CLASS_PER_INTERFACE:
        return("See Interface");
    case USB_CLASS_AUDIO:
        return("Audio");
    case USB_CLASS_COMM:
        return("Communication");
    case USB_CLASS_HID:
        return("Human Interface");
    case USB_CLASS_PRINTER:
        return("Printer");
    case USB_CLASS_MASS_STORAGE:
        return("Mass Storage");
    case USB_CLASS_HUB:
        return("Hub");
    case USB_CLASS_DATA:
        return("CDC Data");
    case USB_CLASS_VENDOR_SPEC:
        return("Vendor specific");
    default :
        return("");
    }
}


void usb_display_class_sub(unsigned char dclass,unsigned char subclass,unsigned char proto)
{
    switch(dclass) 
    {
    case USB_CLASS_PER_INTERFACE:
        s_UartPrint("See Interface");
        break;
    case USB_CLASS_HID:
        s_UartPrint("Human Interface, Subclass: ");
        switch(subclass) 
        {
        case USB_SUB_HID_NONE:
            s_UartPrint("None");
            break;
        case USB_SUB_HID_BOOT:
            s_UartPrint("Boot ");
            switch(proto) 
            {
            case USB_PROT_HID_NONE:
                s_UartPrint("None");
                break;
            case USB_PROT_HID_KEYBOARD:
                s_UartPrint("Keyboard");
                break;
            case USB_PROT_HID_MOUSE:
                s_UartPrint("Mouse");
                break;
            default:
                s_UartPrint("reserved");
            }
            break;
        default:
            s_UartPrint("reserved");
        }
        break;
    case USB_CLASS_MASS_STORAGE:
        s_UartPrint("Mass Storage, ");
        switch(subclass) 
        {
        case US_SC_RBC:
            s_UartPrint("RBC ");
            break;
        case US_SC_8020:
            s_UartPrint("SFF-8020i (ATAPI)");
            break;
        case US_SC_QIC:
            s_UartPrint("QIC-157 (Tape)");
            break;
        case US_SC_UFI:
            s_UartPrint("UFI");
            break;
        case US_SC_8070:
            s_UartPrint("SFF-8070");
            break;
        case US_SC_SCSI:
            s_UartPrint("Transp. SCSI");
            break;
        default:
            s_UartPrint("reserved");
            break;
        }
        s_UartPrint(", ");
        switch(proto) 
        {
        case US_PR_CB:
            s_UartPrint("Command/Bulk");
            break;
        case US_PR_CBI:
            s_UartPrint("Command/Bulk/Int");
            break;
        case US_PR_BULK:
            s_UartPrint("Bulk only");
            break;
        default:
            s_UartPrint("reserved");
        }
        break;
    default:
        s_UartPrint("%s",usb_get_class_desc(dclass));
    }
}


void usb_display_string(struct usb_device *dev,int index)
{
    char buffer[256];
    if (index!=0) 
    {
        if (usb_string(dev,index,&buffer[0],256)>0);
        s_UartPrint("String: \"%s\"",buffer);
    }
}


void usb_display_desc(struct usb_device *dev)
{
    if (dev->descriptor.bDescriptorType==USB_DT_DEVICE) 
    {
        s_UartPrint("%d: %s,  USB Revision %x.%x\r\n",dev->devnum,usb_get_class_desc(dev->config.if_desc[0].bInterfaceClass),
        (dev->descriptor.bcdUSB>>8) & 0xff,dev->descriptor.bcdUSB & 0xff);
        if (strlen(dev->mf) || strlen(dev->prod) || strlen(dev->serial))
        s_UartPrint(" - %s %s %s\r\n",dev->mf,dev->prod,dev->serial);
        if (dev->descriptor.bDeviceClass) 
        {
            s_UartPrint(" - Class: ");
            usb_display_class_sub(dev->descriptor.bDeviceClass,
                dev->descriptor.bDeviceSubClass,
                dev->descriptor.bDeviceProtocol);
            s_UartPrint("\r\n");
        }
        else 
        {
            s_UartPrint(" - Class: (from Interface) %s\r\n",usb_get_class_desc(dev->config.if_desc[0].bInterfaceClass));
        }
        s_UartPrint(" - PacketSize: %d  Configurations: %d\r\n",
            dev->descriptor.bMaxPacketSize0,
            dev->descriptor.bNumConfigurations);
        s_UartPrint(" - Vendor: 0x%04x  Product 0x%04x Version %d.%d\r\n",
            dev->descriptor.idVendor,
            dev->descriptor.idProduct,
            (dev->descriptor.bcdDevice>>8) & 0xff,
            dev->descriptor.bcdDevice & 0xff);
    }
}


void usb_display_conf_desc(struct usb_config_descriptor *config,struct usb_device *dev)
{
    s_UartPrint("   Configuration: %d\r\n",config->bConfigurationValue);
    s_UartPrint("   - Interfaces: %d %s%s%dmA\r\n",config->bNumInterfaces,(config->bmAttributes & 0x40) ? "Self Powered " : "Bus Powered ",
    (config->bmAttributes & 0x20) ? "Remote Wakeup " : "",config->MaxPower*2);
    if (config->iConfiguration) 
    {
        s_UartPrint("   - ");
        usb_display_string(dev,config->iConfiguration);
        s_UartPrint("\r\n");
    }
}


void usb_display_if_desc(struct usb_interface_descriptor *ifdesc,struct usb_device *dev)
{
    s_UartPrint("     Interface: %d\r\n",ifdesc->bInterfaceNumber);
    s_UartPrint("     - Alternate Settings %d, Endpoints: %d\r\n",ifdesc->bAlternateSetting,ifdesc->bNumEndpoints);
    s_UartPrint("     - Class ");
    usb_display_class_sub(ifdesc->bInterfaceClass,ifdesc->bInterfaceSubClass,ifdesc->bInterfaceProtocol);
    s_UartPrint("\r\n");
    if (ifdesc->iInterface) 
    {
        s_UartPrint("     - ");
        usb_display_string(dev,ifdesc->iInterface);
        s_UartPrint("\r\n");
    }
}


void usb_display_ep_desc(struct usb_endpoint_descriptor *epdesc)
{
    s_UartPrint("     - Endpoint %d %s ",epdesc->bEndpointAddress & 0xf,(epdesc->bEndpointAddress & 0x80) ? "In" : "Out");
    switch((epdesc->bmAttributes & 0x03))
    {
        case 0: s_UartPrint("Control"); break;
        case 1: s_UartPrint("Isochronous"); break;
        case 2: s_UartPrint("Bulk"); break;
        case 3: s_UartPrint("Interrupt"); break;
    }
    s_UartPrint(" MaxPacket %d",epdesc->wMaxPacketSize);
    if ((epdesc->bmAttributes & 0x03)==0x3)
    s_UartPrint(" Interval %dms",epdesc->bInterval);
    s_UartPrint("\r\n");
}


/* main routine to diasplay the configs, interfaces and endpoints */
void usb_display_config(struct usb_device *dev)
{
    struct usb_config_descriptor *config;
    struct usb_interface_descriptor *ifdesc;
    struct usb_endpoint_descriptor *epdesc;
    int i,ii;
    config= &dev->config;
    usb_display_conf_desc(config,dev);
    for(i=0;i<config->no_of_if;i++) 
    {
        ifdesc= &config->if_desc[i];
        usb_display_if_desc(ifdesc,dev);
        for(ii=0;ii<ifdesc->no_of_ep;ii++) 
        {
            epdesc= &ifdesc->ep_desc[ii];
            usb_display_ep_desc(epdesc);
        }
    }
    s_UartPrint("\r\n");
}


/* shows the device tree recursively */
void usb_show_tree_graph(struct usb_device *dev,char *pre)
{
    int i,index;
    int has_child,last_child,port;
    index=strlen(pre);
    s_UartPrint(" %s",pre);
    /* check if the device has connected children */
    has_child=0;
    for(i=0;i<dev->maxchild;i++) 
    {
        if (dev->children[i]!=NULL)
        has_child=1;
    }
    /* check if we are the last one */
    last_child=1;
    if (dev->parent!=NULL) 
    {
        for(i=0;i<dev->parent->maxchild;i++) 
        {
            /* search for children */
            if (dev->parent->children[i]==dev) 
            {
                /* found our pointer, see if we have a little sister */
                port=i;
                while(i++<dev->parent->maxchild) 
                {
                    if (dev->parent->children[i]!=NULL) 
                    {
                        /* found a sister */
                        last_child=0;
                        break;
                    } 
                    /* if */
                } 
                /* while */
            } 
            /* device found */
        } 
        /* for all children of the parent */
        s_UartPrint("\b+-");
        /* correct last child */
        if (last_child) 
        {
            pre[index-1]=' ';
        }
    } 
    /* if not root hub */
    else
    s_UartPrint(" ");
    s_UartPrint("%d ",dev->devnum);
    pre[index++]=' ';
    pre[index++]= has_child ? '|' : ' ';
    pre[index]=0;
    s_UartPrint(" %s (%s, %dmA)\r\n",usb_get_class_desc(dev->config.if_desc[0].bInterfaceClass),
    dev->slow ? "1.5MBit/s" : "12MBit/s",dev->config.MaxPower * 2);
    if (strlen(dev->mf) ||
    strlen(dev->prod) ||
    strlen(dev->serial))
    s_UartPrint(" %s  %s %s %s\r\n",pre,dev->mf,dev->prod,dev->serial);
    s_UartPrint(" %s\r\n",pre);
    if (dev->maxchild>0) 
    {
        for(i=0;i<dev->maxchild;i++) 
        {
            if (dev->children[i]!=NULL) 
            {
                usb_show_tree_graph(dev->children[i],pre);
                pre[index]=0;
            }
        }
    }
}


/* main routine for the tree command */
void usb_show_tree(struct usb_device *dev)
{
    char preamble[32];
    memset(preamble,0,32);
    usb_show_tree_graph(dev,&preamble[0]);
}


/******************************************************************************
 * usb boot command intepreter. Derived from diskboot
 */
/*
int do_usbboot (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	char *boot_device = NULL;
	char *ep;
	int dev, part=1, rcode;
	unsigned long addr, cnt, checksum;
	disk_partition_t info;
	image_header_t *hdr;
	block_dev_desc_t *stor_dev;
	switch (argc) {
	case 1:
		addr = CFG_LOAD_ADDR;
		boot_device = getenv ("bootdevice");
		break;
	case 2:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = getenv ("bootdevice");
		break;
	case 3:
		addr = simple_strtoul(argv[1], NULL, 16);
		boot_device = argv[2];
		break;
	default:
		s_UartPrint ("Usage:\r\n%s\r\n", cmdtp->usage);
		return 1;
	}
	if (!boot_device) {
		s_UartPrint("\r\n** No boot device **\r\n");
		return 1;
	}
	dev = simple_strtoul(boot_device, &ep, 16);
	stor_dev=usb_stor_get_dev(dev);
	if (stor_dev->type == DEV_TYPE_UNKNOWN) {
		s_UartPrint("\r\n** Device %d not available\r\n", dev);
		return 1;
	}
	if (stor_dev->block_read==NULL) {
		s_UartPrint("storage device not initialized. Use usb scan\r\n");
		return 1;
	}
	if (*ep) {
		if (*ep != ':') {
			s_UartPrint ("\r\n** Invalid boot device, use `dev[:part]' **\r\n");
			return 1;
		}
		part = simple_strtoul(++ep, NULL, 16);
	}
	if (get_partition_info (stor_dev, part, &info)) {
		// try to boot raw ....//
		strncpy((char *)&info.type[0], BOOT_PART_TYPE, sizeof(BOOT_PART_TYPE));
		strncpy((char *)&info.name[0], "Raw", 4);
		info.start=0;
		info.blksz=0x200;
		info.size=2880;
		s_UartPrint("error reading partinfo...try to boot raw\r\n");
	}
	if ((strncmp((char *)info.type, BOOT_PART_TYPE, sizeof(info.type)) != 0) &&
	    (strncmp((char *)info.type, BOOT_PART_COMP, sizeof(info.type)) != 0)) {
		s_UartPrint ("\r\n** Invalid partition type \"%.32s\""
			" (expect \"" BOOT_PART_TYPE "\")\r\n",
			info.type);
		return 1;
	}
	s_UartPrint("\r\nLoading from USB device %d, partition %d: "
		"Name: %.32s  Type: %.32s\r\n",
		dev, part, info.name, info.type);
	debug ("First Block: %ld,  # of blocks: %ld, Block Size: %ld\r\n",
		info.start, info.size, info.blksz);
	if (stor_dev->block_read(dev, info.start, 1, (ulong *)addr) != 1) {
		s_UartPrint("** Read error on %d:%d\r\n", dev, part);
		return 1;
	}
	hdr = (image_header_t *)addr;
	if (ntohl(hdr->ih_magic) != IH_MAGIC) {
		s_UartPrint("\r\n** Bad Magic Number **\r\n");
		return 1;
	}
	checksum = ntohl(hdr->ih_hcrc);
	hdr->ih_hcrc = 0;
	if (crc32 (0, (uchar *)hdr, sizeof(image_header_t)) != checksum) {
		s_UartPrint("\r\n** Bad Header Checksum **\r\n");
		return 1;
	}
	hdr->ih_hcrc = htonl(checksum);	// restore checksum for later use //
	print_image_hdr (hdr);
	cnt = (ntohl(hdr->ih_size) + sizeof(image_header_t));
	cnt += info.blksz - 1;
	cnt /= info.blksz;
	cnt -= 1;
	if (stor_dev->block_read (dev, info.start+1, cnt,
		      (ulong *)(addr+info.blksz)) != cnt) {
		s_UartPrint("\r\n** Read error on %d:%d\r\n", dev, part);
		return 1;
	}
	// Loading ok, update default load address //
	load_addr = addr;
	flush_cache (addr, (cnt+1)*info.blksz);
	// Check if we should attempt an auto-start //
	if (((ep = getenv("autostart")) != NULL) && (strcmp(ep,"yes") == 0)) {
		char *local_args[2];
		extern int do_bootm (cmd_tbl_t *, int, int, char *[]);
		local_args[0] = argv[0];
		local_args[1] = NULL;
		s_UartPrint("Automatic boot of image at addr 0x%08lX ...\r\n", addr);
		rcode=do_bootm (cmdtp, 0, 1, local_args);
		return rcode;
	}
	return 0;
}


*/
/*
int s_usbhost_reset(void);
int s_usbhost_start(void);
int s_usbhost_stop(void);
int s_usbhost_tree(void);
int s_usbhost_scan(void);
int s_usbhost_stor(void);
int s_usbhost_part(void);
int s_usbhost_info(int argc, char *argv[]);
int s_usbhost_read(int argc, char *argv[]);
int s_usbhost_dev(int argc, char *argv[]);
"reset - reset (rescan) USB controller\r\n"
"usb stop [f]  - stop USB [f]=force stop\r\n"
"usb tree  - show USB device tree\r\n"
"usb info [dev] - show available USB devices\r\n"
"usb storage  - show details of USB storage devices\r\n"
"usb dev [dev] - show or set current USB storage device\r\n"
"usb part [dev] - print partition table of one or all USB storage devices\r\n"
"usb read addr blk# cnt - read `cnt' blocks starting at block `blk#'\r\n"
"    to memory address `addr'\r\n"
*/
int s_usbhost_reset(void)
{
    int i;
    // struct usb_device *dev = NULL;
    // block_dev_desc_t *stor_dev;
     
    usb_stop();
    s_UartPrint("[%d] Reset USB...\r\n", __LINE__);
    i = usb_init();
    /* try to recognize storage devices immediately */
    if (i >= 0) {
        usb_stor_curr_dev = usb_stor_scan(1);
    }
    if(usb_stor_curr_dev < 0) {
        APP_WARN("No USB Storage Device(s) found!!!\r\n");
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

int s_usbhost_scan(void)
{
     
    //s_UartPrint("  NOTE: this command is obsolete and will be phased out\r\n");
    //s_UartPrint("  please use 'usb storage' for USB storage devices information\r\n\r\n");
    usb_stor_curr_dev = usb_stor_scan(1);
    usb_stor_info();
     
    return 0;
}



int s_usbhost_stop(void)
{
     
    s_UartPrint("stopping USB..\r\n");
    usb_stop();
     
    return 0;
}


int s_usbhost_tree(void)
{
     
    s_UartPrint("\r\nDevice Tree:\r\n");
    usb_show_tree(usb_get_dev_index(0));
     
    return 0;
}


int s_usbhost_info(int argc, char *argv[])
{
    int i;
    struct usb_device *dev = NULL;
    //block_dev_desc_t *stor_dev;
    int d;
     
    if (argc==2) 
    {
        for(d=0;d<USB_MAX_DEVICE;d++) 
        {
            dev=usb_get_dev_index(d);
            if (dev==NULL)
            break;
            usb_display_desc(dev);
            usb_display_config(dev);
        }
         
        return 0;
    }
    else 
    {
        i=simple_strtoul(argv[2], NULL, 16);
        s_UartPrint("config for device %d\r\n",i);
        for(d=0;d<USB_MAX_DEVICE;d++) 
        {
            dev=usb_get_dev_index(d);
            if (dev==NULL)
            break;
            if (dev->devnum==i)
            break;
        }
        if (dev==NULL) 
        {
            s_UartPrint("*** NO Device avaiable ***\r\n");
             
            return 0;
        }
        else 
        {
            usb_display_desc(dev);
            usb_display_config(dev);
        }
    }
     
    return 0;
}


//int s_usbhost_stor(void)
//{
//	 
//	usb_stor_info();
//	 
//	return 0;
//}
//int s_usbhost_part(void)
//{
//    int i;
//    //struct usb_device *dev = NULL;
//    block_dev_desc_t *stor_dev;
//    int devno, ok;
//     
//    for (ok=0, devno=0; devno<USB_MAX_STOR_DEV; ++devno) 
//    {
//        stor_dev=usb_stor_get_dev(devno);
//        if (stor_dev->type!=DEV_TYPE_UNKNOWN) 
//        {
//            ok++;
//            if (devno)
//            s_UartPrint("\r\n");
//            s_UartPrint("print_part of %x\r\n",devno);
//            //print_part(stor_dev);
//        }
//    }
//     
//    if (!ok) 
//    {
//        s_UartPrint("\r\nno USB devices available\r\n");
//        return 1;
//    }
//    return 0;
//}
//

/*display the usb status, storage device message if there is have one*/
/*
 *set current device
 */
int s_usbhost_dev(int dev)
{
    block_dev_desc_t *stor_dev;
    s_UartPrint("USB device %d: \r\n", dev);
    if (dev >= USB_MAX_STOR_DEV) 
    {
        APP_WARN("unknown device\r\n");
        return 1;
    }
    stor_dev = usb_stor_get_dev(dev);
     
    if (stor_dev->type == DEV_TYPE_UNKNOWN) 
    {
        APP_WARN("unknown device type\r\n");
        return 1;
    }
    usb_stor_curr_dev = dev;
    s_UartPrint("set %d is now current device\r\n", usb_stor_curr_dev);
    return 0;   
}


int s_usbhost_read(int dev, unsigned long rblk,unsigned long rcnt,unsigned char* rbuffer)
{
    unsigned long addr =(unsigned long)rbuffer;
    unsigned long blk  = rblk;
    unsigned long cnt  = rcnt;
    unsigned long have_read;

    if(s_usbhost_dev(dev) == 1) {
        return 1;
    }

    APP_DEBUG("USB read: device %d block # %ld, count %ld  addr %08x... \r\n",\
    dev, blk, cnt, addr);
    have_read = usb_stor_read(dev, blk, cnt, (ulong *)addr);
     
    if (have_read != cnt)
    {
        APP_ERROR("%ld blocks read: ERROR\r\n", have_read);
        return 1;
    }
    APP_DEBUG("%ld blocks read: OK\r\n", have_read);
    return 0;
}


///*********************************************************************************
// * usb command intepreter
// */
//int do_usb (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
//{
//    int i;
//    struct usb_device *dev = NULL;
//    block_dev_desc_t *stor_dev;
//    if ((strncmp(argv[1], "reset", 5) == 0) ||
//    (strncmp(argv[1], "start", 5) == 0))
//    {
//        usb_stop();
//        s_UartPrint("(Re)start USB...\r\n");
//        i = usb_init();
//        /* try to recognize storage devices immediately */
//        if (i >= 0)
//        usb_stor_curr_dev = usb_stor_scan(1);
//        return 0;
//    }
//    if (strncmp(argv[1],"stop",4) == 0) 
//    {
//        s_UartPrint("stopping USB..\r\n");
//        usb_stop();
//        return 0;
//    }
//    if (strncmp(argv[1],"tree",4) == 0) 
//    {
//        s_UartPrint("\r\nDevice Tree:\r\n");
//        usb_show_tree(usb_get_dev_index(0));
//        return 0;
//    }
//    if (strncmp(argv[1],"inf",3) == 0) 
//    {
//        int d;
//        if (argc==2) 
//        {
//            for(d=0;d<USB_MAX_DEVICE;d++) 
//            {
//                dev=usb_get_dev_index(d);
//                if (dev==NULL)
//                break;
//                usb_display_desc(dev);
//                usb_display_config(dev);
//            }
//            return 0;
//        }
//        else 
//        {
//            int d;
//            i=simple_strtoul(argv[2], NULL, 16);
//            s_UartPrint("config for device %d\r\n",i);
//            for(d=0;d<USB_MAX_DEVICE;d++) 
//            {
//                dev=usb_get_dev_index(d);
//                if (dev==NULL)
//                break;
//                if (dev->devnum==i)
//                break;
//            }
//            if (dev==NULL) 
//            {
//                s_UartPrint("*** NO Device avaiable ***\r\n");
//                return 0;
//            }
//            else 
//            {
//                usb_display_desc(dev);
//                usb_display_config(dev);
//            }
//        }
//        return 0;
//    }
//    if (strncmp(argv[1], "scan", 4) == 0) 
//    {
//        s_UartPrint("  NOTE: this command is obsolete and will be phased out\r\n");
//        s_UartPrint("  please use 'usb storage' for USB storage devices information\r\n\r\n");
//        usb_stor_info();
//        return 0;
//    }
//    if (strncmp(argv[1], "stor", 4) == 0) 
//    {
//        usb_stor_info();
//        return 0;
//    }
//    if (strncmp(argv[1],"part",4) == 0) 
//    {
//        int devno, ok;
//        for (ok=0, devno=0; devno<USB_MAX_STOR_DEV; ++devno) 
//        {
//            stor_dev=usb_stor_get_dev(devno);
//            if (stor_dev->type!=DEV_TYPE_UNKNOWN) 
//            {
//                ok++;
//                if (devno)
//                s_UartPrint("\r\n");
//                s_UartPrint("print_part of %x\r\n",devno);
//                print_part(stor_dev);
//            }
//        }
//        if (!ok) 
//        {
//            s_UartPrint("\r\nno USB devices available\r\n");
//            return 1;
//        }
//        return 0;
//    }
//    if (strcmp(argv[1],"read") == 0) 
//    {
//        if (usb_stor_curr_dev<0) 
//        {
//            s_UartPrint("no current device selected\r\n");
//            return 1;
//        }
//        if (argc==5) 
//        {
//            unsigned long addr = simple_strtoul(argv[2], NULL, 16);
//            unsigned long blk  = simple_strtoul(argv[3], NULL, 16);
//            unsigned long cnt  = simple_strtoul(argv[4], NULL, 16);
//            unsigned long n;
//            s_UartPrint("\r\nUSB read: device %d block # %ld, count %ld ... ",
//            usb_stor_curr_dev, blk, cnt);
//            stor_dev=usb_stor_get_dev(usb_stor_curr_dev);
//            n = stor_dev->block_read(usb_stor_curr_dev, blk, cnt, (ulong *)addr);
//            s_UartPrint("%ld blocks read: %s\r\n",n,(n==cnt) ? "OK" : "ERROR");
//            if (n==cnt)
//            return 0;
//            return 1;
//        }
//    }
//    if (strncmp(argv[1], "dev", 3) == 0) 
//    {
//        if (argc == 3) 
//        {
//            int dev = (int)simple_strtoul(argv[2], NULL, 10);
//            s_UartPrint("\r\nUSB device %d: ", dev);
//            if (dev >= USB_MAX_STOR_DEV) 
//            {
//                s_UartPrint("unknown device\r\n");
//                return 1;
//            }
//            s_UartPrint("\r\n    Device %d: ", dev);
//            stor_dev=usb_stor_get_dev(dev);
//            dev_print(stor_dev);
//            if (stor_dev->type == DEV_TYPE_UNKNOWN) 
//            {
//                return 1;
//            }
//            usb_stor_curr_dev = dev;
//            s_UartPrint("... is now current device\r\n");
//            return 0;
//        }
//        else 
//        {
//            s_UartPrint ("\r\nUSB device %d: ", usb_stor_curr_dev);
//            stor_dev=usb_stor_get_dev(usb_stor_curr_dev);
//            dev_print(stor_dev);
//            if (stor_dev->type == DEV_TYPE_UNKNOWN) 
//            {
//                return 1;
//            }
//            return 0;
//        }
//        return 0;
//    }
//    s_UartPrint("Usage:\r\n%s\r\n", cmdtp->usage);
//    return 1;
//}

/* end of file*/