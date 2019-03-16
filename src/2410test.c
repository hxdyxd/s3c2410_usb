#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"
#include "mmu.h"
#include "uart.h"
#include "timer.h"
//#include "rtcapi.h"
#include "cmd_usb.h"

//by hxdyxd
#include "app_debug.h"

/************************************************************/


//======================================================
void HaltUndef(void)
{
    printf("Undefined instruction exception!!!\r\n");
    while(1);
}

void HaltSwi(void)
{
    printf("SWI exception!!!\r\n");
    while(1);
}

void HaltPabort(void)
{
    printf("Pabort exception!!!\r\n");
    while(1);
}

void HaltDabort(void)
{
    printf("Dabort exception!!!\r\n");
    while(1);
}

void Isr_Init(void)
{
    pISR_UNDEF  = (U32)HaltUndef;
    pISR_SWI    = (U32)HaltSwi;
    pISR_PABORT = (U32)HaltPabort;
    pISR_DABORT = (U32)HaltDabort;
    rINTMOD = 0x0;    // All=IRQ mode
    rINTMSK = BIT_ALLMSK;     // All interrupt is masked.        
}



void doDelay10us(unsigned int i) {
    i *= 20;
    while(--i);
}

int Main(void)
{
    int j = 0;
    char cmd;
    char tempbuf[10][100];
    unsigned char buffer[10000];
    
#if 0           //bank0 modified to RO_START  
    MMU_Init(); //MMU should be reconfigured or turned off for the debugger, 
    //After downloading, MMU should be turned off for the MMU based program,such as WinCE.  
#else
    MMU_EnableICache();
#endif
    
//  ChangeClockDivider(1, 1);    // 1:2:4
//  ChangeMPllValue(192, 4, 1);    //FCLK=180.0Mhz
    SetClockDivider(1, 1);
    SetSysFclk(DFT_FCLK_VAL);
    Delay( 0 ) ;
    
    Port_Init();
    Isr_Init();
    
    Uart_Select(0);
    Uart_Init(0, UART_BAUD);
    
    //RequestBiosTimerEvent(10, Led1Flash); //when request, auto open bios timer
    //RequestBiosTimerEvent(20, Led2Flash);
    //RequestBiosTimerEvent(50, Led3Flash);
    
    //RequestBiosTimerEvent(100, Led4Flash);
        
    //GPIO,UART0,PWM TIMER,NAND FLASH
//  DisableModuleClock(CLOCK_ALL);
    EnableModuleClock(CLOCK_UART0|CLOCK_TIMER|CLOCK_GPIO|CLOCK_NAND|CLOCK_LCD);


    putch('\r\n');
    puts("***********************************\r\n");
    puts("*                                 *\r\n");
    puts("*    FS2410 board demo program    *\r\n");
    puts("*    Version: 2.1   2005/10/12    *\r\n");
    puts("*                                 *\r\n");  
    puts("***********************************\r\n");
    ChangeUPllValue(40, 4, 1);  //UCLK=48Mhz   
    s_usbhost_reset();
    
    
    APP_DEBUG("Usb Host Example!\r\n");
    
    
    while(1)
    {
        printf("---------------------\r\n");
        printf("0-(Re)start 1-Start       2-Stop   3-Tree \r\n");
        printf("4-Scan      5-Build-Tree  7-Info   8-Read\r\n");
        printf("---------------------\r\n");
        
        printf("\r\nPlease Select Function Key:\r\n");
        cmd = s_getkey();
        //cmd = '1';
        //doDelay10us(100000);
        s_UartPrint("\r\n");
        switch(cmd)
        {
        case '0':
            APP_DEBUG("s_usbhost_reset() \r\n");
            s_usbhost_reset();
            break;
        case '1':
            APP_DEBUG("s_usbhost_start() \r\n");
            s_usbhost_start();
            break;
        case '2':
            APP_DEBUG("s_usbhost_stop() \r\n");
            s_usbhost_stop();
            break;
        case '3':
            APP_DEBUG("s_usbhost_tree() \r\n");
            s_usbhost_tree();
            break;
        case '4':
            APP_DEBUG("s_usbhost_scan() \r\n");
            s_usbhost_scan();
            break;
        case '5':
            APP_DEBUG("usb_scan_devices() \r\n");
            usb_scan_devices();
            break;
        case '7':
            APP_DEBUG("s_usbhost_info(2, tempbuf) \r\n");
            s_usbhost_info(2, (char **)tempbuf);
            break;
        case '8':
            APP_DEBUG("s_usbhost_read(0, 0, 1, buffer) \r\n");
            s_usbhost_read(0, 0, 1, buffer);
            for(j = 0; j < 0x200; j++)
            {
                printf("%02X ", buffer[j]);
            }
            printf("\r\n");

            break;
        }
    }

    return 0;
}
