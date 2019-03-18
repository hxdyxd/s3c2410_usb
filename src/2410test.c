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



int Main(void)
{
    unsigned char buffer[10000];
    
#if 0           //bank0 modified to RO_START  
    MMU_Init(); //MMU should be reconfigured or turned off for the debugger, 
    //After downloading, MMU should be turned off for the MMU based program,such as WinCE.  
#else
    MMU_EnableICache();
#endif
    
    SetClockDivider(1, 1);
    SetSysFclk(DFT_FCLK_VAL);
    Delay( 0 ) ;
    
    Port_Init();
    Isr_Init();
    
    Uart_Select(0);
    Uart_Init(0, UART_BAUD);

    EnableModuleClock(CLOCK_UART0|CLOCK_TIMER|CLOCK_GPIO|CLOCK_NAND|CLOCK_LCD);


    putch('\r\n');
    puts("***********************************\r\n");
    puts("*    FS2410 board demo program    *\r\n");
    puts("*    Version: 2.1   2005/10/12    *\r\n"); 
    puts("***********************************\r\n");
    ChangeUPllValue(40, 4, 1);  //UCLK=48Mhz

    APP_DEBUG("Usb Host Example!\r\n");
    
    while(1)
    {
        APP_WARN("s_getkey() \r\n");
        if(0) {
            s_getkey();
        } else {
            delay: {
                int i = 10000000;
                while(--i);
            }
        }

        APP_WARN("s_usbhost_reset() \r\n");
        if(s_usbhost_reset() < 0) {
            APP_WARN("No USB Storage Device(s) found!!!\r\n");
            continue;
        }

        APP_WARN("s_usbhost_read(0, 0, 1, buffer) \r\n");
        if(s_usbhost_read(0, 0, 1, buffer) < 0) {
            APP_WARN("Read Failed!!!\r\n");
            continue;
        }

        if(1) {
            int j;
            for(j = 0; j < 0x200; j++)
            {
                printf("%02X ", buffer[j]);
            }
            printf("\r\n");
        }
    }

    return 0;
}
