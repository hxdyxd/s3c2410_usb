#include "def.h"
#include "2410addr.h"
#include "2410lib.h"

extern U8 USB_OR_UART_Download_OK ;
extern U32 downloadAddress, downloadFileSize;
static U8 *temp;

static void __irq Uart0RxInt(void)
{
    ClearSubPending(BIT_SUB_RXD0); //rSUBSRCPND = BIT_SUB_RXD0;          //Clear pending bit (Requested)
    ClearPending(BIT_UART0);

    *temp ++= RdURXH0(); 
}

void comdownload(void)
{
	U32 size;
	U8 *buf;
	U16 checksum;
	void (*run)();
	
	puts("\r\nNow download file from uart0...\r\n");
	downloadAddress = _NONCACHE_STARTADDRESS;
	buf  = (U8 *)downloadAddress;
	temp = buf-4;		
	
	pISR_UART0 = (U32)Uart0RxInt;		//串口接收数据中断
	ClearSubPending(BIT_SUB_RXD0);
	ClearPending(BIT_UART0);
	EnableSubIrq(BIT_SUB_RXD0);
	EnableIrq(BIT_UART0);	

	while((U32)temp<(U32)buf)		
    {
        Led_Display(0);
        Delay(4000);
        Led_Display(15);
        Delay(4000);
    }							//接收文件长度,4 bytes	
	
	size  = *(U32 *)(buf-4);
	downloadFileSize = size-6;
    printf("Download File Size = %d\r\n", size);
	
	while(((U32)temp-(U32)buf)<(size-4))
	{		
		Led_Display(0);
        Delay(4000);
        Led_Display(15);
        Delay(4000);		
	}
	
	DisableSubIrq(BIT_SUB_RXD0);
	DisableIrq(BIT_UART0);
	
	checksum = 0;
	for(size=0; size<downloadFileSize; size++)
		checksum += buf[size];
	if(checksum!=(buf[size]|(buf[size+1]<<8))) {
		puts("Checksum fail!\r\n");
		return;
	}
	
	if( downloadFileSize > 0 )
		USB_OR_UART_Download_OK = 1 ;

	puts("Are you sure to run? [y/n]\r\n");
	while(1)
	{
		U8 key = getch();
		
		if(key=='n')
			return;
			
		if(key=='y')
		{
			run = (void (*)())buf;
			(*run)();
		}
	}
	
}

