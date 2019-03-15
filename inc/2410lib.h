//===================================================================
// File Name : 2410lib.h
// Function  : S3C2410 
// Program   : Shin, On Pil (SOP)
// Date      : February 26, 2002
// Version   : 0.0
// History
//  0.0 :Feb.20.2002:SOP     : Programming start
//  0.01:Mar.29.2002:purnnamu: For POWEROFF_wake_up, the START... label is added
//===================================================================

#ifndef __2410lib_h__
#define __2410lib_h__

#ifdef __cplusplus
extern "C" {
#endif

#include "def.h"

#define DebugOut Uart_Printf

#define min(x1,x2) (((x1)<(x2))? (x1):(x2))
#define max(x1,x2) (((x1)>(x2))? (x1):(x2))

#define ONESEC0 (62500)	             //16us resolution, max 1.04 sec
#define ONESEC1 (31250)	             //32us resolution, max 2.09 sec
#define ONESEC2 (15625)	             //64us resolution, max 4.19 sec
#define ONESEC3 (7812)	             //128us resolution, max 8.38 sec
#define ONESEC4 (PCLK/128/(0xff+1))  //@60Mhz, 128*4us resolution, max 32.53 sec

#define NULL 0

#define EnterPWDN(clkcon) ((void (*)(int))0x20)(clkcon)
void StartPointAfterPowerOffWakeUp(void); //purnnamu:Mar.29.2002


// 2410lib.c
void Delay(int time);              //Watchdog Timer is used.

void *malloc(unsigned nbyte); 
void free(void *pt);

void Port_Init(void);
void Uart_Select(int ch);
void Uart_TxEmpty(int ch);
void Uart_Init(int mclk,int baud);
char getch(void);
char getkey(void);
int  Uart_GetIntNum(void);
int Uart_GetIntNum_GJ(void) ;
void putch(int data);
void printf(char *fmt,...);
void puts(char *pt);
#define	Uart_Printf	printf
#define	Uart_GetKey	getkey
#define	Uart_Getch	getch
char DbgPause(char *str);
__inline void NullPrintf(char *fmt,...){}
#ifdef __DEBUG_VERSION
#define	DbgOut	printf
#else
#define	DbgOut	NullPrintf
#endif
void wait_print_end(void);

void Timer_Start(int divider);    //Watchdog Timer is used.
int  Timer_Stop(void);            //Watchdog Timer is used.

void Led_Display(int data);
void Beep(U32 freq, U32 ms) ;
void BUZZER_PWM_Test( void ) ;

void ChangeMPllValue(int m,int p,int s);
void ChangeClockDivider(int hdivn,int pdivn);
void ChangeUPllValue(int m,int p,int s);

#ifdef __cplusplus
}
#endif

#endif  //__2410lib_h__
