
#include "s3c2410.h"
#ifndef   _P78_BASE_H
#define   _P78_BASE_H


#define   INT_ADC     31  
#define   INT_RTC     30  
#define   INT_SPI1    29 
#define   INT_UART0   28
#define   INT_IIC     27  
#define   INT_USBH    26 
#define   INT_USBD    25 
#define   INT_UART1   23
#define   INT_SPI0    22 
#define   INT_SDI     21  
#define   INT_DMA3    20 
#define   INT_DMA2    19 
#define   INT_DMA1    18 
#define   INT_DMA0    17  
#define   INT_UART2   15
#define   INT_TIMER4  14
#define   INT_TIMER3  13
#define   INT_TIMER2  12
#define   INT_TIMER1  11
#define   INT_TIMER0  10
#define   INT_WDT     9
#define   INT_TICK    8
#define   EINT8_23    5
#define   EINT4_7     4
#define   EINT3       3
#define   EINT2       2
#define   EINT1       1
#define   EINT0       0
#define  CONCS1       0x18000000
#define  CONCS2       0x20000000
//define for concs2 port
#define  LCDLED        0x01
#define  LCDRST        0x02
#define  MCRPWR        0x08
#define  LEDTXD        0x10
#define  LEDRXD        0x20
#define  LEDONLINE     0x40
#define  LOW           0
#define  HIGH          1
//Function: Set user IRQ handler
//IntNo:    can be above
//Handler:  user handler
void s_SetIRQHandler(int IntNo, void (*Handler)(void));
//Function: Set user FIQ handler
//Handler:  user handler
void s_SetFIQHandler(void (*Handler)(void));
//Function: Init uart0 (115200,8,n,1, no interrupt)
//Handler:  user handler
void s_UartInit(void);
//Function: Receive a char from  uart0 until time out
//TimeOut:  time out (ms)
//Return:   0 success   -1 Failed(Time out)
/*
#define MEGA	(1000000)

#define FIN 	(12000000)	//hzh


void cal_cpu_bus_clk(void);
void Uart_Init(int pclk,int baud);
*/



int s_UartRecv(unsigned char *ch, int TimeOut);
//Function: Send a char to  uart0
void s_UartSend( unsigned char ch);
//Function: Receive a char from uart0
unsigned char s_getkey(void);
//Send formated strings to uart0
void s_UartPrint(char *fmt,...);
void s_WriteConCs1(unsigned char mode);
void s_WriteConCs2(unsigned char addr,unsigned char mode);
#endif
