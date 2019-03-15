
#include   <stdarg.h>
#include   "base.h"

//by hxdyxd
#include "app_debug.h"


//#include   "posapi.h"
#define  CONCS_DATA32
static volatile unsigned char ConCs1,ConCs2;
typedef void IRQ_HANDLER(void);
typedef IRQ_HANDLER *pIRQ_HANDLER;
IRQ_HANDLER *FIQ_HANDLER, *IRQ_HANDLER_TABLE[32];
void AbortHandler(void);
//extern void s_LcdInit1(void);
//extern void DelayMs(unsigned short ms);
void __gccmain()
{
}


// void s_WriteConCs1(unsigned char mode)
// {
//     #ifdef CONCS_DATA32
//     *(volatile int*)(CONCS1) =(int)(mode<<24);
//     #else   
//     *(volatile char *)CONCS1=mode;
//     #endif
// }


// void s_WriteConCs2(unsigned char addr,unsigned char mode)
// {
//     if(mode)    ConCs2|=addr;
//     else        ConCs2&=~addr;
//     #ifdef CONCS_DATA32
//     *(volatile int*)(CONCS2) =(int)(ConCs2<<24);
//     #else   
//     *(volatile char*)CONCS2=ConCs2;
//     #endif
// }


// void s_SetIRQHandler(int IntNo, void (*Handler)(void))
// {
//     IRQ_HANDLER_TABLE[IntNo] = Handler; 
// }


// void s_SetFIQHandler(void (*Handler)(void))
// {
//     FIQ_HANDLER = Handler; 
// }


// void s_IRQHandlerInit(void)
// {
//     int i;
//     for(i=0;i<32;i++)
//     s_SetIRQHandler(i, AbortHandler);
//     ConCs1=0xff;
//     ConCs2=0xff;
//     APP_DEBUG("s_IRQHandlerInit\r\n");
// }


// void s_UartInit(void)
// {
//  rULCON0 = 0x03;
//     rUCON0 = 0x05;
//     rUFCON0 = 0x00;
//     rUMCON0 = 0x00;
//     rUBRDIV0 = 55;
// }

// int s_UartRecv(unsigned char *ch, int TimeOut)
// {
//     unsigned long cnt;
//     cnt=TimeOut*7619;
//     while(!(rUFSTAT0 & 0x000F))
//     {
//         if(cnt-- ==0) return -1;
//     }
//     *ch = rURXH0;
//     return 0;
// }


void s_UartSend( unsigned char ch)
{
    rUTXH0 = ch;    
    while(!(rUTRSTAT0 & 0x04));
}


unsigned char s_getkey(void)
{
    while(!(rUFSTAT0 & 0x000F));
    return rURXH0;
}


void s_UartPrint(char *fmt,...)
{
    int i;
    va_list marker;
    char buff[1024];
    //s_UartInit();
    buff[0]=0;
    va_start( marker, fmt);
    vsprintf(buff,fmt,marker);
    va_end( marker );
    for(i=0;buff[i];i++) s_UartSend(buff[i]);
}


// void s_Beep(void)
// {
//         rGPBCON &= 0xfffffffc;
//         rGPBCON |= 0x00000002;//SET TOUT0 FUNCTION;
//         rGPBUP  &= 0xfffffffe;      
//         rTCNTB0 = 470;    
//         rTCMPB0 = 470/2;  
//         rTCON |= 0x0f;
//         rTCON &= ~(1<<1);               
//         DelayMs(100);       
//         rTCON &= ~(0x0b);       
//         rGPBCON &= 0xfffffffc;
//         rGPBCON |= 0x00000001;//SET GPB0=0;
//         rGPBUP  &= 0xfffffffe;
//         rGPBDAT &= 0xfffffffe; 
// }


void UndefinedHandler(void)
{
    //int i;
    //s_Beep();
    //s_LcdInit1();
    //ScrPrint(0,6,1,"Undefined fail!");
    //for(i=0;i<10;i++){
    s_UartPrint("Undefined failed!");
    //}
    for(;;);
}


void PrefetchAbortHandler(void)
{
    int i;
    //s_Beep();
    //s_LcdInit1();
    // ScrPrint(0,6,1,"Prefetch fail!");
    //for(i=0;i<10;i++){
    s_UartPrint("Prefetch failed!");
    //}
    for(;;);
}


void DataAbortHandler(void)
{
    int i;
    //s_Beep();
    //s_LcdInit1();
    // ScrPrint(0,6,1,"DataAbort fail!");
    //for(i=0;i<10;i++){
    s_UartPrint("DataAbort failed!");
    //}
    for(;;);
}


void AbortHandler(void)
{
    int i;
    //s_Beep();
    //s_LcdInit1();
    //ScrPrint(0,6,1,"  Abort failed!");
    //for(i=0;i<10;i++){
    s_UartPrint("Abort failed!");
    //}
    for(;;);
}


