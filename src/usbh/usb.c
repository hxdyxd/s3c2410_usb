
/*
 *
 * Most of this source has been derived from the Linux USB
 * project:
 * (C) Copyright Linus Torvalds 1999
 * (C) Copyright Johannes Erdfelt 1999-2001
 * (C) Copyright Andreas Gal 1999
 * (C) Copyright Gregory P. Smith 1999
 * (C) Copyright Deti Fliegl 1999 (new USB architecture)
 * (C) Copyright Randy Dunlap 2000
 * (C) Copyright David Brownell 2000 (kernel hotplug, usb_device_id)
 * (C) Copyright Yggdrasil Computing, Inc. 2000
 *     (usb_device_id matching changes by Adam J. Richter)
 *
 * Adapted for U-Boot:
 * (C) Copyright 2001 Denis Peter, MPL AG Switzerland
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
/*
 * How it works:
 *
 * Since this is a bootloader, the devices will not be automatic
 * (re)configured on hotplug, but after a restart of the USB the
 * device should work.
 *
 * For each transfer (except "Interrupt") we wait for completion.
 */
//#include <common.h>
//#include <command.h>
//#include <asm/processor.h>
//#include <linux/ctype.h>
//#include <string.h>
//#include "ctype.h"
#include "usb.h"
#include "base.h"
#include "s3c2410.h"

//by hxdyxd
#include "app_debug.h"

//#undef USB_DEBUG
#define  USB_DEBUG
#ifdef	USB_DEBUG
#define	USB_PRINTF s_UartPrint
#else
#define USB_PRINTF()
#endif
#define USB_BUFSIZ	512
static struct usb_device usb_dev[USB_MAX_DEVICE];
static int dev_index;
static int running;
static int asynch_allowed;
static struct devrequest setup_packet;
extern void hc_interrupt_irq (void);
/**********************************************************************
 * some forward declerations...
 */
void usb_scan_devices(void);
int  usb_hub_probe(struct usb_device *dev, int ifnum);
void usb_hub_reset(void);
void udelay (unsigned long usec)
{
    unsigned long j,i;
    //for(i=0;i<50;i++);   
    j=usec/1000;
    for(i=0;i<j;i++)
    {
        rINTMSK |= (1<<13);
        rTCON &= ~(0x0f<<16);  //stop timer3
        rSRCPND = (1<<13);		   
        rTCNTB3 = 2028;         //1ms=2028
        rTCMPB3=0;
        rTCON &= ~(0x0f<<16);   //STOP TIMER 3
        rTCON |= (0x02<<16); // AUTOLOAD=0 update=1
        rTCON &= ~(1<<17);   //CLEAR MANUAL UPDATE BIT
        rTCON |= (1<<16);    //START
        while((rSRCPND & (1<<13))==0);
        rSRCPND = (1<<13);	
    }
}


/***********************************************************************
 * wait_ms
 */
void __inline wait_ms(unsigned long ms)
{
    while(ms-->0)
    udelay(1000);
}


void TestWaitMs(void)
{
    int i,j;
    s_UartPrint("Test Wait Ms... Any Key to Start...\r\n");
    s_getkey();
    j=0;
    for(i=0;i<120;i++)
    {
        s_UartPrint("j=%d sec\r\n",j);
        wait_ms(1000);  
        j++;
    }
}


/***************************************************************************
 * Init USB Device
 */
int usb_init(void)
{
    int result;
    s_SetIRQHandler(INT_USBH, hc_interrupt_irq );
    running=0;
    dev_index=0;
    asynch_allowed=1;
    usb_hub_reset();
    /* init low_level USB */
    //s_UartPrint("USB:   ");
    result = usb_lowlevel_init();
    /* if lowlevel init is OK, scan the bus for devices i.e. search HUBs and configure them */
    if(result==0) 
    {
        
        running=1;
        usb_scan_devices();
        return 0;
    }
    else 
    {
        s_UartPrint("Error, couldn't init Lowlevel part\r\n");
        return -1;
    }
}


/******************************************************************************
 * Stop USB this stops the LowLevel Part and deregisters USB devices.
 */
int usb_stop(void)
{
    asynch_allowed=1;
	
    usb_hub_reset();
    return usb_lowlevel_stop();
}


/*
 * disables the asynch behaviour of the control message. This is used for data
 * transfers that uses the exclusiv access to the control and bulk messages.
 */
void usb_disable_asynch(int disable)
{
    asynch_allowed=!disable;
}


/*-------------------------------------------------------------------
 * Message wrappers.
 *
 */
/*
 * submits an Interrupt Message
 */
int usb_submit_int_msg(struct usb_device *dev, unsigned long pipe,
void *buffer,int transfer_len, int interval)
{
    return submit_int_msg(dev,pipe,buffer,transfer_len,interval);
}


/*
 * submits a control message and waits for comletion (at least timeout * 1ms)
 * If timeout is 0, we don't wait for completion (used as example to set and
 * clear keyboards LEDs). For data transfers, (storage transfers) we don't
 * allow control messages with 0 timeout, by previousely resetting the flag
 * asynch_allowed (usb_disable_asynch(1)).
 * returns the transfered length if OK or -1 if error. The transfered length
 * and the current status are stored in the dev->act_len and dev->status.
 */
int usb_control_msg(struct usb_device *dev, unsigned int pipe,
unsigned char request, unsigned char requesttype,
unsigned short value, unsigned short index,
void *data, unsigned short size, int timeout)
{
    //s_UartPrint("asynch_allowed=%d",asynch_allowed);
    if((timeout==0)&&(!asynch_allowed)) /* request for a asynch control pipe is not allowed */
    return -1;
    /* set setup command */
    setup_packet.requesttype = requesttype;
    setup_packet.request = request;
    setup_packet.value = swap_16(value);
    setup_packet.index = swap_16(index);
    setup_packet.length = swap_16(size);
    //APP_DEBUG("usb_control_msg: request: 0x%X, requesttype: 0x%X\r\nvalue 0x%X index 0x%X length 0x%X pipe 0x%X \r\n",
    //request,requesttype,value,index,size,pipe);
    dev->status=USB_ST_NOT_PROC; /*not yet processed */
    submit_control_msg(dev,pipe,data,size,&setup_packet);
    if(timeout==0) 
    {
        return (int)size;
    }
    while(timeout--) 
    {
        if(!((volatile unsigned long)dev->status & USB_ST_NOT_PROC))
        break;
        wait_ms(1);
    }
    if(dev->status==0)
    return dev->act_len;
    else 
    {
        return -1;
    }
}


/*-------------------------------------------------------------------
 * submits bulk message, and waits for completion. returns 0 if Ok or
 * -1 if Error.
 * synchronous behavior
 */
int usb_bulk_msg(struct usb_device *dev, unsigned int pipe,
void *data, int len, int *actual_length, int timeout)
{
    if (len < 0)
    return -1;
    dev->status=USB_ST_NOT_PROC; /*not yet processed */
    submit_bulk_msg(dev,pipe,data,len);
    while(timeout--) 
    {
        if(!((volatile unsigned long)dev->status & USB_ST_NOT_PROC))
        break;
        wait_ms(1);
    }
    *actual_length=dev->act_len;
    if(dev->status==0)
    return 0;
    else
    return -1;
}


/*-------------------------------------------------------------------
 * Max Packet stuff
 */
/*
 * returns the max packet size, depending on the pipe direction and
 * the configurations values
 */
int usb_maxpacket(struct usb_device *dev,unsigned long pipe)
{
    if((pipe & USB_DIR_IN)==0) /* direction is out -> use emaxpacket out */
    return(dev->epmaxpacketout[((pipe>>15) & 0xf)]);
    else
    return(dev->epmaxpacketin[((pipe>>15) & 0xf)]);
}


/*
 * set the max packed value of all endpoints in the given configuration
 */
int usb_set_maxpacket(struct usb_device *dev)
{
    int i,ii,b;
    struct usb_endpoint_descriptor *ep;
    for(i=0; i<dev->config.bNumInterfaces;i++) 
    {
        for(ii=0; ii<dev->config.if_desc[i].bNumEndpoints; ii++) 
        {
            ep=&dev->config.if_desc[i].ep_desc[ii];
            b=ep->bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
            if((ep->bmAttributes & USB_ENDPOINT_XFERTYPE_MASK)==USB_ENDPOINT_XFER_CONTROL) 
            {
                /* Control => bidirectional */
                dev->epmaxpacketout[b] = ep->wMaxPacketSize;
                dev->epmaxpacketin [b] = ep->wMaxPacketSize;
                USB_PRINTF("##Control EP epmaxpacketout/in[%d] = %d\r\n",b,dev->epmaxpacketin[b]);
            }
            else 
            {
                if ((ep->bEndpointAddress & 0x80)==0) 
                {
                    /* OUT Endpoint */
                    if(ep->wMaxPacketSize > dev->epmaxpacketout[b]) 
                    {
                        dev->epmaxpacketout[b] = ep->wMaxPacketSize;
                        USB_PRINTF("##EP epmaxpacketout[%d] = %d\r\n",b,dev->epmaxpacketout[b]);
                    }
                }
                else  
                {
                    /* IN Endpoint */
                    if(ep->wMaxPacketSize > dev->epmaxpacketin[b]) 
                    {
                        dev->epmaxpacketin[b] = ep->wMaxPacketSize;
                        USB_PRINTF("##EP epmaxpacketin[%d] = %d\r\n",b,dev->epmaxpacketin[b]);
                    }
                } 
                /* if out */
            } 
            /* if control */
        } 
        /* for each endpoint */
    }
    return 0;
}


/*******************************************************************************
 * Parse the config, located in buffer, and fills the dev->config structure.
 * Note that all little/big endian swapping are done automatically.
 */
int usb_parse_config(struct usb_device *dev, unsigned char *buffer, int cfgno)
{
    struct usb_descriptor_header *head;
    int index,ifno,epno;
    ifno=-1;
    epno=-1;
    dev->configno=cfgno;
    head =(struct usb_descriptor_header *)&buffer[0];
    if(head->bDescriptorType!=USB_DT_CONFIG) 
    {
        APP_ERROR(" ERROR: NOT USB_CONFIG_DESC %x\r\n",head->bDescriptorType);
        return -1;
    }
    memcpy(&dev->config,buffer,buffer[0]);
    dev->config.wTotalLength=swap_16(dev->config.wTotalLength);
    dev->config.no_of_if=0;
    index=dev->config.bLength;
    /* Ok the first entry must be a configuration entry, now process the others */
    head=(struct usb_descriptor_header *)&buffer[index];
    while(index+1 < dev->config.wTotalLength) 
    {
        switch(head->bDescriptorType) 
        {
        case USB_DT_INTERFACE:
            ifno=dev->config.no_of_if;
            dev->config.no_of_if++; /* found an interface desc, increase numbers */
            memcpy(&dev->config.if_desc[ifno],&buffer[index],buffer[index]); /* copy new desc */
            dev->config.if_desc[ifno].no_of_ep=0;
            break;
        case USB_DT_ENDPOINT:
            epno=dev->config.if_desc[ifno].no_of_ep;
            dev->config.if_desc[ifno].no_of_ep++; /* found an endpoint */
            memcpy(&dev->config.if_desc[ifno].ep_desc[epno],&buffer[index],buffer[index]);
            dev->config.if_desc[ifno].ep_desc[epno].wMaxPacketSize
            =swap_16(dev->config.if_desc[ifno].ep_desc[epno].wMaxPacketSize);
            USB_PRINTF("if %d, ep %d\r\n",ifno,epno);
            break;
        default:
            if(head->bLength==0)
            return 1;
            USB_PRINTF("unknown Description Type : %x\r\n",head->bDescriptorType);
            {
                int i;
                unsigned char *ch;
                ch=(unsigned char *)head;
                for(i=0;i<head->bLength; i++)
                USB_PRINTF("%02X ",*ch++);
                USB_PRINTF("\r\n\r\n\r\n");
            }
            break;
        }
        index+=head->bLength;
        head=(struct usb_descriptor_header *)&buffer[index];
    }
    return 1;
}


/***********************************************************************
 * Clears an endpoint
 * endp: endpoint number in bits 0-3;
 * direction flag in bit 7 (1 = IN, 0 = OUT)
 */
int usb_clear_halt(struct usb_device *dev, int pipe)
{
    int result;
    int endp = usb_pipeendpoint(pipe)|(usb_pipein(pipe)<<7);
    result = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_CLEAR_FEATURE, USB_RECIP_ENDPOINT, 0, endp, NULL, 0, USB_CNTL_TIMEOUT * 3);
    /* don't clear if failed */
    if (result < 0)
    return result;
    /*
    	 * NOTE: we do not get status and verify reset was successful
    	 * as some devices are reported to lock up upon this check..
    	 */
    usb_endpoint_running(dev, usb_pipeendpoint(pipe), usb_pipeout(pipe));
    /* toggle is reset on clear */
    usb_settoggle(dev, usb_pipeendpoint(pipe), usb_pipeout(pipe), 0);
    return 0;
}


/**********************************************************************
 * get_descriptor type
 */
int usb_get_descriptor(struct usb_device *dev, unsigned char type, unsigned char index, void *buf, int size)
{
    int res;
    res = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_DESCRIPTOR, USB_DIR_IN,
    (type << 8) + index, 0,
    buf, size, USB_CNTL_TIMEOUT);
    return res;
}


/**********************************************************************
 * gets configuration cfgno and store it in the buffer
 */
int usb_get_configuration_no(struct usb_device *dev,unsigned char *buffer,int cfgno)
{
    int result;
    unsigned int tmp;
    struct usb_config_descriptor *config;
    config=(struct usb_config_descriptor *)&buffer[0];
    result = usb_get_descriptor(dev, USB_DT_CONFIG, cfgno, buffer, 8);
    if (result < 8) 
    {
        if (result < 0)
        s_UartPrint("unable to get descriptor, error %lX\r\n",dev->status);
        else
        s_UartPrint("config descriptor too short (expected %i, got %i)\r\n",8,result);
        return -1;
    }
    tmp=swap_16(config->wTotalLength);
    if (tmp > USB_BUFSIZ) 
    {
        USB_PRINTF("usb_get_configuration_no: failed to get descriptor - too long: %d\r\n",
        tmp);
        return -1;
    }
    result = usb_get_descriptor(dev, USB_DT_CONFIG, cfgno, buffer, tmp);
    USB_PRINTF("get_conf_no %d Result %d, wLength %d\r\n",cfgno,result,tmp);
    return result;
}


/********************************************************************
 * set address of a device to the value in dev->devnum.
 * This can only be done by addressing the device via the default address (0)
 */
int usb_set_address(struct usb_device *dev)
{
    int res;
    APP_DEBUG("set address %d\r\n",dev->devnum);
    res=usb_control_msg(dev, usb_snddefctrl(dev),
    USB_REQ_SET_ADDRESS, 0,
    (dev->devnum),0,
    NULL,0, USB_CNTL_TIMEOUT);
    return res;
}


/********************************************************************
 * set interface number to interface
 */
int usb_set_interface(struct usb_device *dev, int interface, int alternate)
{
    struct usb_interface_descriptor *if_face = NULL;
    int ret, i;
    for (i = 0; i < dev->config.bNumInterfaces; i++) 
    {
        if (dev->config.if_desc[i].bInterfaceNumber == interface) 
        {
            if_face = &dev->config.if_desc[i];
            break;
        }
    }
    if (!if_face) 
    {
        s_UartPrint("selecting invalid interface %d", interface);
        return -1;
    }
    if ((ret = usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_SET_INTERFACE, USB_RECIP_INTERFACE, alternate,
    interface, NULL, 0, USB_CNTL_TIMEOUT * 5)) < 0)
    return ret;
    return 0;
}


/********************************************************************
 * set configuration number to configuration
 */
int usb_set_configuration(struct usb_device *dev, int configuration)
{
    int res;
    APP_DEBUG("set configuration %d\r\n", configuration);
    /* set setup command */
    res=usb_control_msg(dev, usb_sndctrlpipe(dev,0),
    USB_REQ_SET_CONFIGURATION, 0,
    configuration,0,
    NULL,0, USB_CNTL_TIMEOUT);
    if(res==0) 
    {
        dev->toggle[0] = 0;
        dev->toggle[1] = 0;
        return 0;
    }
    else
    return -1;
}


/********************************************************************
 * set protocol to protocol
 */
int usb_set_protocol(struct usb_device *dev, int ifnum, int protocol)
{
    return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_SET_PROTOCOL, USB_TYPE_CLASS | USB_RECIP_INTERFACE,
    protocol, ifnum, NULL, 0, USB_CNTL_TIMEOUT);
}


/********************************************************************
 * set idle
 */
int usb_set_idle(struct usb_device *dev, int ifnum, int duration, int report_id)
{
    return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_SET_IDLE, USB_TYPE_CLASS | USB_RECIP_INTERFACE,
    (duration << 8) | report_id, ifnum, NULL, 0, USB_CNTL_TIMEOUT);
}


/********************************************************************
 * get report
 */
int usb_get_report(struct usb_device *dev, int ifnum, unsigned char type, unsigned char id, void *buf, int size)
{
    return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_REPORT, USB_DIR_IN | USB_TYPE_CLASS | USB_RECIP_INTERFACE,
    (type << 8) + id, ifnum, buf, size, USB_CNTL_TIMEOUT);
}


/********************************************************************
 * get class descriptor
 */
int usb_get_class_descriptor(struct usb_device *dev, int ifnum,
unsigned char type, unsigned char id, void *buf, int size)
{
    return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_DESCRIPTOR, USB_RECIP_INTERFACE | USB_DIR_IN,
    (type << 8) + id, ifnum, buf, size, USB_CNTL_TIMEOUT);
}


/********************************************************************
 * get string index in buffer
 */
int usb_get_string(struct usb_device *dev, unsigned short langid, unsigned char index, void *buf, int size)
{
    int i;
    int result;
    for (i = 0; i < 3; ++i) 
    {
        /* some devices are flaky */
        result = usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
        USB_REQ_GET_DESCRIPTOR, USB_DIR_IN,
        (USB_DT_STRING << 8) + index, langid, buf, size,
        USB_CNTL_TIMEOUT);
        if (result > 0)
        break;
    }
    return result;
}


static void usb_try_string_workarounds(unsigned char *buf, int *length)
{
    int newlength, oldlength = *length;
    for (newlength = 2; newlength + 1 < oldlength; newlength += 2)
    if (!isprint(buf[newlength]) || buf[newlength + 1])
    break;
    if (newlength > 2) 
    {
        buf[0] = newlength;
        *length = newlength;
    }
}


static int usb_string_sub(struct usb_device *dev, unsigned int langid,
unsigned int index, unsigned char *buf)
{
    int rc;
    /* Try to read the string descriptor by asking for the maximum
    	 * possible number of bytes */
    rc = usb_get_string(dev, langid, index, buf, 255);
    /* If that failed try to read the descriptor length, then
    	 * ask for just that many bytes */
    if (rc < 2) 
    {
        rc = usb_get_string(dev, langid, index, buf, 2);
        if (rc == 2)
        rc = usb_get_string(dev, langid, index, buf, buf[0]);
    }
    if (rc >= 2) 
    {
        if (!buf[0] && !buf[1])
        usb_try_string_workarounds(buf, &rc);
        /* There might be extra junk at the end of the descriptor */
        if (buf[0] < rc)
        rc = buf[0];
        rc = rc - (rc & 1); /* force a multiple of two */
    }
    if (rc < 2)
    rc = -1;
    return rc;
}


/********************************************************************
 * usb_string:
 * Get string index and translate it to ascii.
 * returns string length (> 0) or error (< 0)
 */
int usb_string(struct usb_device *dev, int index, char *buf, size_t size)
{
    unsigned char mybuf[USB_BUFSIZ];
    unsigned char *tbuf;
    int err;
    unsigned int u, idx;
    if (size <= 0 || !buf || !index)
    return -1;
    buf[0] = 0;
    tbuf=&mybuf[0];
    /* get langid for strings if it's not yet known */
    if (!dev->have_langid) 
    {
        err = usb_string_sub(dev, 0, 0, tbuf);
        if (err < 0) 
        {
            USB_PRINTF("error getting string descriptor 0 (error=%x)\r\n",dev->status);
            return -1;
        } 
        else if (tbuf[0] < 4) 
        {
            USB_PRINTF("string descriptor 0 too short\r\n");
            return -1;
        } 
        else 
        {
            dev->have_langid = -1;
            dev->string_langid = tbuf[2] | (tbuf[3]<< 8);
            /* always use the first langid listed */
            APP_DEBUG("USB device number %d default language ID 0x%x\r\n",
            dev->devnum, dev->string_langid);
        }
    }
    err = usb_string_sub(dev, dev->string_langid, index, tbuf);
    if (err < 0)
    return err;
    size--;		/* leave room for trailing NULL char in output buffer */
    for (idx = 0, u = 2; u < err; u += 2) 
    {
        if (idx >= size)
        break;
        if (tbuf[u+1])			/* high byte */
        buf[idx++] = '?';  /* non-ASCII character */
        else
        buf[idx++] = tbuf[u];
    }
    buf[idx] = 0;
    err = idx;
    return err;
}


/********************************************************************
 * USB device handling:
 * the USB device are static allocated [USB_MAX_DEVICE].
 */
/* returns a pointer to the device with the index [index].
 * if the device is not assigned (dev->devnum==-1) returns NULL
 */
struct usb_device * usb_get_dev_index(int index)
{
    if(usb_dev[index].devnum==-1)
    return NULL;
    else
    return &usb_dev[index];
}


/* returns a pointer of a new device structure or NULL, if
 * no device struct is available
 */
struct usb_device * usb_alloc_new_device(void)
{
    int i;
    APP_DEBUG("New Device %d\r\n", dev_index);
    if(dev_index==USB_MAX_DEVICE) 
    {
        APP_ERROR("ERROR, too many USB Devices, max=%d\r\n",USB_MAX_DEVICE);
        return NULL;
    }
    usb_dev[dev_index].devnum=dev_index+1; /* default Address is 0, real addresses start with 1 */
    usb_dev[dev_index].maxchild=0;
    for(i=0;i<USB_MAXCHILDREN;i++)
    usb_dev[dev_index].children[i]=NULL;
    usb_dev[dev_index].parent=NULL;
    dev_index++;
    return &usb_dev[dev_index-1];
}


#define MAX_TRIES 5
static int hub_port_reset(struct usb_device *dev, int port,
unsigned short *portstat)
{
    int tries;
    struct usb_port_status portsts;
    unsigned short portstatus, portchange;
    APP_DEBUG("hub_port_reset: resetting port %d...\r\n", port);
    for(tries=0;tries<MAX_TRIES;tries++) 
    {
        usb_set_port_feature(dev, port + 1, USB_PORT_FEAT_RESET);
        wait_ms(200);
        if (usb_get_port_status(dev, port + 1, &portsts)<0) 
        {
            APP_ERROR("get_port_status failed status %lX\r\n",dev->status);
            return -1;
        }
        portstatus = swap_16(portsts.wPortStatus);
        portchange = swap_16(portsts.wPortChange);
        APP_DEBUG("portstatus %x, change %x, %s\r\n", portstatus ,portchange,
            (portstatus&(1<<USB_PORT_FEAT_LOWSPEED)) ? "Low Speed" : "High Speed");
        APP_DEBUG("STAT_C_CONNECTION = %d STAT_CONNECTION = %d  USB_PORT_STAT_ENABLE %d\r\n",
            (portchange & USB_PORT_STAT_C_CONNECTION) ? 1 : 0,
            (portstatus & USB_PORT_STAT_CONNECTION) ? 1 : 0,
            (portstatus & USB_PORT_STAT_ENABLE) ? 1 : 0);

        if ((portchange & USB_PORT_STAT_C_CONNECTION) ||
        !(portstatus & USB_PORT_STAT_CONNECTION)) {
            return -1;
        }
        if (portstatus & USB_PORT_STAT_ENABLE) 
        {
            break;
        }
        wait_ms(200);
    }
    if (tries==MAX_TRIES) 
    {
        APP_ERROR("Cannot enable port %i after %i retries, disabling port.\r\n", port+1, MAX_TRIES);
        APP_ERROR("Maybe the USB cable is bad?\r\n");
        return -1;
    }
    usb_clear_port_feature(dev, port + 1, USB_PORT_FEAT_C_RESET);
    *portstat = portstatus;
    return 0;
}


/*
 * By the time we get here, the device has gotten a new device ID
 * and is in the default state. We need to identify the thing and
 * get the ball rolling..
 *
 * Returns 0 for success, != 0 for error.
 */
int usb_new_device(struct usb_device *dev)
{
    int addr, err;
    int tmp;
    unsigned char tmpbuf[USB_BUFSIZ];
    dev->descriptor.bMaxPacketSize0 = 8;  /* Start off at 8 bytes  */
    dev->maxpacketsize = 0;		/* Default to 8 byte max packet size */
    dev->epmaxpacketin [0] = 8;
    dev->epmaxpacketout[0] = 8;
    /* We still haven't set the Address yet */
    addr = dev->devnum;
    dev->devnum = 0;
    #undef NEW_INIT_SEQ
    #ifdef NEW_INIT_SEQ
    /* this is a Windows scheme of initialization sequence, with double
    	 * reset of the device. Some equipment is said to work only with such
    	 * init sequence; this patch is based on the work by Alan Stern:
    	 * http://sourceforge.net/mailarchive/forum.php?thread_id=5729457&forum_id=5398
         * Whoops, we can't find that page. by hxdyxd 190315
    	 */
    {
    int j;
    struct usb_device_descriptor *desc;
    int port = -1;
    struct usb_device *parent = dev->parent;
    unsigned short portstatus;
    /* send 64-byte GET-DEVICE-DESCRIPTOR request.  Since the descriptor is
    	 * only 18 bytes long, this will terminate with a short packet.  But if
    	 * the maxpacket size is 8 or 16 the device may be waiting to transmit
    	 * some more. */
    desc = (struct usb_device_descriptor *)tmpbuf;
    desc->bMaxPacketSize0 = 0;
    for (j = 0; j < 3; ++j) 
    {
        err = usb_get_descriptor(dev, USB_DT_DEVICE, 0, desc, 64);
        if (err < 0) 
        {
            USB_PRINTF("usb_new_device: 64 byte descr\r\n");
            break;
        }
    }
    dev->descriptor.bMaxPacketSize0 = desc->bMaxPacketSize0;
    /* find the port number we're at */
    if (parent) 
    {
        for (j = 0; j < parent->maxchild; j++) 
        {
            if (parent->children[j] == dev) 
            {
                port = j;
                break;
            }
        }
        if (port < 0) 
        {
            s_UartPrint("usb_new_device: cannot locate device's port..\r\n");
            return 1;
        }
        /* reset the port for the second time */
        err = hub_port_reset(dev->parent, port, &portstatus);
        if (err < 0) 
        {
            s_UartPrint("\r\n     Couldn't reset port %i\r\n", port);
            return 1;
        }
    }
    }
    #else
    APP_DEBUG("and this is the old and known way of initializing devices\r\n");
    /* and this is the old and known way of initializing devices */
    //s_UartPrint("NEW_step 1\r\n");
    err = usb_get_descriptor(dev, USB_DT_DEVICE, 0, &dev->descriptor, 8);
    if (err < 8) 
    {
        s_UartPrint("\r\n      USB device not responding, giving up (status=%lX)\r\n",dev->status);
        return 1;
    }
    //s_UartPrint("NEW_step 3 err=%d\r\n",err);
    #endif
    dev->epmaxpacketin [0] = dev->descriptor.bMaxPacketSize0;
    dev->epmaxpacketout[0] = dev->descriptor.bMaxPacketSize0;
    switch (dev->descriptor.bMaxPacketSize0) 
    {
        case 8: dev->maxpacketsize = 0; break;
        case 16: dev->maxpacketsize = 1; break;
        case 32: dev->maxpacketsize = 2; break;
        case 64: dev->maxpacketsize = 3; break;
    }
    dev->devnum = addr;
    //s_UartPrint("NEW_step 4\r\n");
    err = usb_set_address(dev); /* set address */
    //s_UartPrint("NEW_step 5\r\n");
    if (err < 0) 
    {
        s_UartPrint("\r\n      USB device not accepting new address (error=%lX)\r\n", dev->status);
        s_UartPrint("NEW_step 6\r\n");
        return 1;
    }
    wait_ms(10);	/* Let the SET_ADDRESS settle */
    tmp = sizeof(dev->descriptor);
    //tmp=8;	
    //s_UartPrint("NEW_step 7\r\n");
    err = usb_get_descriptor(dev, USB_DT_DEVICE, 0, &dev->descriptor, sizeof(dev->descriptor));
    if (err < tmp) 
    {
        if (err < 0)
        s_UartPrint("unable to get device descriptor (error=%d)\r\n",err);
        else
        s_UartPrint("USB device descriptor short read (expected %i, got %i)\r\n",tmp,err);
        s_UartPrint("NEW_step 8\r\n");
        return 1;
    }
    //dev->epmaxpacketin [0] = dev->descriptor.bMaxPacketSize0;
    //dev->epmaxpacketout[0] = dev->descriptor.bMaxPacketSize0;
    /* correct le values */
    dev->descriptor.bcdUSB=swap_16(dev->descriptor.bcdUSB);
    dev->descriptor.idVendor=swap_16(dev->descriptor.idVendor);
    dev->descriptor.idProduct=swap_16(dev->descriptor.idProduct);
    dev->descriptor.bcdDevice=swap_16(dev->descriptor.bcdDevice);
    //s_UartPrint("NEW_step 9 size=%d\r\n",sizeof(dev->descriptor));
    /*USB_PRINTF("bLength %x\r\n", dev->descriptor.bLength);
    USB_PRINTF("bDescriptorType      %x\r\n", dev->descriptor.bDescriptorType);
    USB_PRINTF("bcdUSB      %x\r\n", dev->descriptor.bcdUSB);
    USB_PRINTF("bDeviceClass     %x\r\n", dev->descriptor.bDeviceClass);
    USB_PRINTF("bDeviceSubClass     %x\r\n", dev->descriptor.bDeviceSubClass);
    USB_PRINTF("bDeviceProtocol      %x\r\n", dev->descriptor.bDeviceProtocol);
    USB_PRINTF("bMaxPacketSize0      %x\r\n", dev->descriptor.bMaxPacketSize0);
    USB_PRINTF("idVendor      %x\r\n", dev->descriptor.idVendor);
    USB_PRINTF("idProduct      %x\r\n", dev->descriptor.idProduct);
    USB_PRINTF("bcdDevice      %x\r\n", dev->descriptor.bcdDevice);
    USB_PRINTF("iManufacturer      %x\r\n", dev->descriptor.iManufacturer);
    USB_PRINTF("iProduct      %x\r\n", dev->descriptor.iProduct);
    USB_PRINTF("iSerialNumber      %x\r\n", dev->descriptor.iSerialNumber);
    USB_PRINTF("bNumConfigurations      %x\r\n", dev->descriptor.bNumConfigurations);*/
    /* only support for one config for now */
    usb_get_configuration_no(dev,&tmpbuf[0],0);
    //s_UartPrint("NEW_step 10\r\n");
    usb_parse_config(dev,&tmpbuf[0],0);
    //s_UartPrint("NEW_step 11\r\n");
    usb_set_maxpacket(dev);
    // s_UartPrint("NEW_step 12\r\n");
    /* we set the default configuration here */
    if (usb_set_configuration(dev, dev->config.bConfigurationValue)) 
    {
        s_UartPrint("failed to set default configuration len %d, status %lX\r\n",dev->act_len,dev->status);
        return -1;
    }
    USB_PRINTF("new device strings: Mfr=%d, Product=%d, SerialNumber=%d\r\n",
    dev->descriptor.iManufacturer, dev->descriptor.iProduct, dev->descriptor.iSerialNumber);
    memset(dev->mf, 0, sizeof(dev->mf));
    memset(dev->prod, 0, sizeof(dev->prod));
    memset(dev->serial, 0, sizeof(dev->serial));
    if (dev->descriptor.iManufacturer)
    usb_string(dev, dev->descriptor.iManufacturer, dev->mf, sizeof(dev->mf));
    if (dev->descriptor.iProduct)
    usb_string(dev, dev->descriptor.iProduct, dev->prod, sizeof(dev->prod));
    if (dev->descriptor.iSerialNumber)
    usb_string(dev, dev->descriptor.iSerialNumber, dev->serial, sizeof(dev->serial));
    USB_PRINTF("Manufacturer %s\r\n", dev->mf);
    USB_PRINTF("Product      %s\r\n", dev->prod);
    USB_PRINTF("SerialNumber %s\r\n", dev->serial);
    /* now prode if the device is a hub */
    usb_hub_probe(dev,0);
    return 0;
}


/* build device Tree  */
void usb_scan_devices(void)
{
    int i;
    struct usb_device *dev;
    /* first make all devices unknown */
    for(i=0;i<USB_MAX_DEVICE;i++) 
    {        
        memset(&usb_dev[i],0,sizeof(struct usb_device));
        usb_dev[i].devnum=-1;
    }
    dev_index=0;
    /* device 0 is always present (root hub, so let it analyze) */
    //s_UartPrint("scan_step 1\r\n");
    dev=usb_alloc_new_device();
    s_UartPrint("scan_step 2\r\n");
	s_UartPrint("dev=%d",dev);
    usb_new_device(dev);
    //s_UartPrint("scan_step 3\r\n");
    //s_UartPrint("%d USB Device(s) found\r\n",dev_index);
    /* insert "driver" if possible */
}


/****************************************************************************
 * HUB "Driver"
 * Probes device for being a hub and configurate it
 */
#undef	USB_HUB_DEBUG
//#define   USB_HUB_DEBUG
#ifdef	USB_HUB_DEBUG
#define	USB_HUB_PRINTF	s_UartPrint
#else
void USB_HUB_PRINTF(char *fmt,...){}
#endif
static struct usb_hub_device hub_dev[USB_MAX_HUB];
static int usb_hub_index;
int usb_get_hub_descriptor(struct usb_device *dev, void *data, int size)
{
    return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_DESCRIPTOR, USB_DIR_IN | USB_RT_HUB,
    USB_DT_HUB << 8, 0, data, size, USB_CNTL_TIMEOUT);
}


int usb_clear_hub_feature(struct usb_device *dev, int feature)
{
    return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_CLEAR_FEATURE, USB_RT_HUB, feature, 0, NULL, 0, USB_CNTL_TIMEOUT);
}


int usb_clear_port_feature(struct usb_device *dev, int port, int feature)
{
    return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_CLEAR_FEATURE, USB_RT_PORT, feature, port, NULL, 0, USB_CNTL_TIMEOUT);
}


int usb_set_port_feature(struct usb_device *dev, int port, int feature)
{
    return usb_control_msg(dev, usb_sndctrlpipe(dev, 0),
    USB_REQ_SET_FEATURE, USB_RT_PORT, feature, port, NULL, 0, USB_CNTL_TIMEOUT);
}


int usb_get_hub_status(struct usb_device *dev, void *data)
{
    return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_STATUS, USB_DIR_IN | USB_RT_HUB, 0, 0,
    data, sizeof(struct usb_hub_status), USB_CNTL_TIMEOUT);
}


int usb_get_port_status(struct usb_device *dev, int port, void *data)
{
    return usb_control_msg(dev, usb_rcvctrlpipe(dev, 0),
    USB_REQ_GET_STATUS, USB_DIR_IN | USB_RT_PORT, 0, port,
    data, sizeof(struct usb_hub_status), USB_CNTL_TIMEOUT);
}


static void usb_hub_power_on(struct usb_hub_device *hub)
{
    int i;
    struct usb_device *dev;
    dev=hub->pusb_dev;
    /* Enable power to the ports */
    USB_HUB_PRINTF("enabling power on all ports\r\n");
    for (i = 0; i < dev->maxchild; i++) 
    {
        usb_set_port_feature(dev, i + 1, USB_PORT_FEAT_POWER);
        USB_HUB_PRINTF("port %d returns %lX\r\n",i+1,dev->status);
        wait_ms(hub->desc.bPwrOn2PwrGood * 2);
    }
}


void usb_hub_reset(void)
{
    usb_hub_index=0;
}


struct usb_hub_device *usb_hub_allocate(void)
{
    if(usb_hub_index<USB_MAX_HUB) 
    {
        APP_DEBUG("[%d] usb_hub_index = %d\r\n", __LINE__, usb_hub_index);
        return &hub_dev[usb_hub_index++];
    }
    s_UartPrint("ERROR: USB_MAX_HUB (%d) reached\r\n",USB_MAX_HUB);
    return NULL;
}


void usb_hub_port_connect_change(struct usb_device *dev, int port)
{
    struct usb_device *usb;
    struct usb_port_status portsts;
    unsigned short portstatus, portchange;
    /* Check status */
    if (usb_get_port_status(dev, port + 1, &portsts)<0) 
    {
        USB_HUB_PRINTF("get_port_status failed\r\n");
        return;
    }
    portstatus = swap_16(portsts.wPortStatus);
    portchange = swap_16(portsts.wPortChange);
    USB_HUB_PRINTF("portstatus %x, change %x, %s\r\n", portstatus, portchange,
    portstatus&(1<<USB_PORT_FEAT_LOWSPEED) ? "Low Speed" : "High Speed");
    /* Clear the connection change status */
    usb_clear_port_feature(dev, port + 1, USB_PORT_FEAT_C_CONNECTION);
    /* Disconnect any existing devices under this port */
    if (((!(portstatus & USB_PORT_STAT_CONNECTION)) &&
    (!(portstatus & USB_PORT_STAT_ENABLE)))|| (dev->children[port])) 
    {
        USB_HUB_PRINTF("usb_disconnect(&hub->children[port]);\r\n");
        /* Return now if nothing is connected */
        if (!(portstatus & USB_PORT_STAT_CONNECTION))
        return;
    }
    wait_ms(200);
    /* Reset the port */
    if (hub_port_reset(dev, port, &portstatus) < 0) 
    {
        USB_HUB_PRINTF("cannot reset port %i!?\r\n", port + 1);
        return;
    }
    wait_ms(200);
    /* Allocate a new device struct for it */
    usb=usb_alloc_new_device();
    usb->slow = (portstatus & USB_PORT_STAT_LOW_SPEED) ? 1 : 0;
    dev->children[port] = usb;
    usb->parent=dev;
    /* Run it through the hoops (find a driver, etc) */
    if (usb_new_device(usb)) 
    {
        /* Woops, disable the port */
        USB_HUB_PRINTF("hub: disabling port %d\r\n", port + 1);
        usb_clear_port_feature(dev, port + 1, USB_PORT_FEAT_ENABLE);
    }
}


int usb_hub_configure(struct usb_device *dev)
{
    unsigned char buffer[USB_BUFSIZ], *bitmap;
    struct usb_hub_descriptor *descriptor;
    struct usb_hub_status *hubsts;
    int i;
    struct usb_hub_device *hub;
    /* "allocate" Hub device */
    hub=usb_hub_allocate();
    if(hub==NULL)
    return -1;
    hub->pusb_dev=dev;
    /* Get the the hub descriptor */
    if (usb_get_hub_descriptor(dev, buffer, 4) < 0) 
    {
        USB_HUB_PRINTF("usb_hub_configure: failed to get hub descriptor, giving up %lX\r\n",dev->status);
        return -1;
    }
    descriptor = (struct usb_hub_descriptor *)buffer;
    /* silence compiler warning if USB_BUFSIZ is > 256 [= sizeof(char)] */
    i = descriptor->bLength;
    if (i > USB_BUFSIZ) 
    {
        USB_HUB_PRINTF("usb_hub_configure: failed to get hub descriptor - too long: %d\r\n",
        descriptor->bLength);
        return -1;
    }
    if (usb_get_hub_descriptor(dev, buffer, descriptor->bLength) < 0) 
    {
        USB_HUB_PRINTF("usb_hub_configure: failed to get hub descriptor 2nd giving up %lX\r\n",dev->status);
        return -1;
    }
    memcpy((unsigned char *)&hub->desc,buffer,descriptor->bLength);
    /* adjust 16bit values */
    hub->desc.wHubCharacteristics=swap_16(descriptor->wHubCharacteristics);
    /* set the bitmap */
    bitmap=(unsigned char *)&hub->desc.DeviceRemovable[0];
    memset(bitmap,0xff,(USB_MAXCHILDREN+1+7)/8); /* devices not removable by default */
    bitmap=(unsigned char *)&hub->desc.PortPowerCtrlMask[0];
    memset(bitmap,0xff,(USB_MAXCHILDREN+1+7)/8); /* PowerMask = 1B */
    for(i=0;i<((hub->desc.bNbrPorts + 1 + 7)/8);i++) 
    {
        hub->desc.DeviceRemovable[i]=descriptor->DeviceRemovable[i];
    }
    for(i=0;i<((hub->desc.bNbrPorts + 1 + 7)/8);i++) 
    {
        hub->desc.DeviceRemovable[i]=descriptor->PortPowerCtrlMask[i];
    }
    dev->maxchild = descriptor->bNbrPorts;
    USB_HUB_PRINTF("%d ports detected\r\n", dev->maxchild);
    switch (hub->desc.wHubCharacteristics & HUB_CHAR_LPSM) 
    {
    case 0x00:
        USB_HUB_PRINTF("ganged power switching\r\n");
        break;
    case 0x01:
        USB_HUB_PRINTF("individual port power switching\r\n");
        break;
    case 0x02:
    case 0x03:
        USB_HUB_PRINTF("unknown reserved power switching mode\r\n");
        break;
    }
    if (hub->desc.wHubCharacteristics & HUB_CHAR_COMPOUND)
    USB_HUB_PRINTF("part of a compound device\r\n");
    else
    USB_HUB_PRINTF("standalone hub\r\n");
    switch (hub->desc.wHubCharacteristics & HUB_CHAR_OCPM) 
    {
    case 0x00:
        USB_HUB_PRINTF("global over-current protection\r\n");
        break;
    case 0x08:
        USB_HUB_PRINTF("individual port over-current protection\r\n");
        break;
    case 0x10:
    case 0x18:
        USB_HUB_PRINTF("no over-current protection\r\n");
        break;
    }
    USB_HUB_PRINTF("power on to power good time: %dms\r\n", descriptor->bPwrOn2PwrGood * 2);
    USB_HUB_PRINTF("hub controller current requirement: %dmA\r\n", descriptor->bHubContrCurrent);
    for (i = 0; i < dev->maxchild; i++)
    USB_HUB_PRINTF("port %d is%s removable\r\n", i + 1,
    hub->desc.DeviceRemovable[(i + 1)/8] & (1 << ((i + 1)%8)) ? " not" : "");
    if (sizeof(struct usb_hub_status) > USB_BUFSIZ) 
    {
        USB_HUB_PRINTF("usb_hub_configure: failed to get Status - too long: %d\r\n",
        descriptor->bLength);
        return -1;
    }
    if (usb_get_hub_status(dev, buffer) < 0) 
    {
        USB_HUB_PRINTF("usb_hub_configure: failed to get Status %lX\r\n",dev->status);
        return -1;
    }
    hubsts = (struct usb_hub_status *)buffer;
    USB_HUB_PRINTF("get_hub_status returned status %X, change %X\r\n",
    swap_16(hubsts->wHubStatus),swap_16(hubsts->wHubChange));
    USB_HUB_PRINTF("local power source is %s\r\n",
    (swap_16(hubsts->wHubStatus) & HUB_STATUS_LOCAL_POWER) ? "lost (inactive)" : "good");
    USB_HUB_PRINTF("%sover-current condition exists\r\n",
    (swap_16(hubsts->wHubStatus) & HUB_STATUS_OVERCURRENT) ? "" : "no ");
    usb_hub_power_on(hub);
    for (i = 0; i < dev->maxchild; i++) 
    {
        struct usb_port_status portsts;
        unsigned short portstatus, portchange;
        if (usb_get_port_status(dev, i + 1, &portsts) < 0) 
        {
            USB_HUB_PRINTF("get_port_status failed\r\n");
            continue;
        }
        portstatus = swap_16(portsts.wPortStatus);
        portchange = swap_16(portsts.wPortChange);
        USB_HUB_PRINTF("Port %d Status %X Change %X\r\n",i+1,portstatus,portchange);
        if (portchange & USB_PORT_STAT_C_CONNECTION) 
        {
            USB_HUB_PRINTF("port %d connection change\r\n", i + 1);
            usb_hub_port_connect_change(dev, i);
        }
        if (portchange & USB_PORT_STAT_C_ENABLE) 
        {
            USB_HUB_PRINTF("port %d enable change, status %x\r\n", i + 1, portstatus);
            usb_clear_port_feature(dev, i + 1, USB_PORT_FEAT_C_ENABLE);
            /* EM interference sometimes causes bad shielded USB devices to
            			 * be shutdown by the hub, this hack enables them again.
            			 * Works at least with mouse driver */
            if (!(portstatus & USB_PORT_STAT_ENABLE) &&
            (portstatus & USB_PORT_STAT_CONNECTION) && (dev->children[i])) 
            {
                USB_HUB_PRINTF("already running port %i disabled by hub (EMI?), re-enabling...\r\n",
                i + 1);
                usb_hub_port_connect_change(dev, i);
            }
        }
        if (portstatus & USB_PORT_STAT_SUSPEND) 
        {
            USB_HUB_PRINTF("port %d suspend change\r\n", i + 1);
            usb_clear_port_feature(dev, i + 1,  USB_PORT_FEAT_SUSPEND);
        }
        if (portchange & USB_PORT_STAT_C_OVERCURRENT) 
        {
            USB_HUB_PRINTF("port %d over-current change\r\n", i + 1);
            usb_clear_port_feature(dev, i + 1, USB_PORT_FEAT_C_OVER_CURRENT);
            usb_hub_power_on(hub);
        }
        if (portchange & USB_PORT_STAT_C_RESET) 
        {
            USB_HUB_PRINTF("port %d reset change\r\n", i + 1);
            usb_clear_port_feature(dev, i + 1, USB_PORT_FEAT_C_RESET);
        }
    } 
    /* end for i all ports */
    return 0;
}


int usb_hub_probe(struct usb_device *dev, int ifnum)
{
    struct usb_interface_descriptor *iface;
    struct usb_endpoint_descriptor *ep;
    int ret;
    //s_UartPrint("hub_probe_step1\r\n");
    iface = &dev->config.if_desc[ifnum];
    /* Is it a hub? */
    if (iface->bInterfaceClass != USB_CLASS_HUB)
    return 0;
    //s_UartPrint("hub_probe_step2\r\n");
    /* Some hubs have a subclass of 1, which AFAICT according to the */
    /*  specs is not defined, but it works */
    if ((iface->bInterfaceSubClass != 0) &&
    (iface->bInterfaceSubClass != 1))
    return 0;
    //s_UartPrint("hub_probe_step3\r\n");
    /* Multiple endpoints? What kind of mutant ninja-hub is this? */
    if (iface->bNumEndpoints != 1)
    return 0;
    //s_UartPrint("hub_probe_step4\r\n");
    ep = &iface->ep_desc[0];
    /* Output endpoint? Curiousier and curiousier.. */
    if (!(ep->bEndpointAddress & USB_DIR_IN))
    return 0;
    //s_UartPrint("hub_probe_step5\r\n");
    /* If it's not an interrupt endpoint, we'd better punt! */
    if ((ep->bmAttributes & 3) != 3)
    return 0;
    /* We found a hub */
    //s_UartPrint("hub_probe_step6\r\n");
    USB_HUB_PRINTF("USB hub found\r\n");
    ret=usb_hub_configure(dev);
    return ret;
}


//#endif /* (CONFIG_COMMANDS & CFG_CMD_USB) */
/* EOF */
