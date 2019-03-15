#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "2410slib.h"
#include "mmu.h"
#include "uart.h"
#include "timer.h"

/******************************************************************************
	4X4 矩阵键盘
四个输入引脚：	EINT0 -----( GPF0  )----INPUT
				EINT2 -----( GPF2  )----INPUT
				EINT11-----( GPG3  )----INPUT
				EINT19-----( GPG11 )----INPUT
				
四个输出引脚：	KEYSCAN0---( GPE11 )----OUTPUT
				KEYSCAN1---( GPG6  )----OUTPUT
				KEYSCAN2---( GPE13 )----OUTPUT
				KEYSCAN3---( GPG2  )----OUTPUT
******************************************************************************/
U8 Key_Scan( void )
{
	Delay( 50 ) ;
	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2))) | (1<<6) | (0<<2) ;		//GPG6,2 output 0
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11))) | (1<<13) | (1<<11) ;		//GPE13,11 output 0
	if(      (rGPFDAT&(1<< 0)) == 0 )		return 16 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 15 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 14 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 13 ;

	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2))) | (0<<6) | (1<<2) ;		//GPG6,2 output 0
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11))) | (1<<13) | (1<<11) ;		//GPE13,11 output 0
	if(      (rGPFDAT&(1<< 0)) == 0 )		return 11 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 8 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 5 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 2 ;

	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2))) | (1<<6) | (1<<2) ;		//GPG6,2 output 0
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11))) | (1<<13) | (0<<11) ;		//GPE13,11 output 0
	if(      (rGPFDAT&(1<< 0)) == 0 )		return 10 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 7 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 4 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 1 ;

	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2))) | (1<<6) | (1<<2) ;		//GPG6,2 output 0
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11))) | (0<<13) | (1<<11) ;		//GPE13,11 output 0
	if(      (rGPFDAT&(1<< 0)) == 0 )		return 12 ;
	else if( (rGPFDAT&(1<< 2)) == 0 )		return 9 ;
	else if( (rGPGDAT&(1<< 3)) == 0 )		return 6 ;
	else if( (rGPGDAT&(1<<11)) == 0 )		return 3 ;
	else return 0xff ;
}


static void __irq KeyISR(void)
{
	U8 key ;

	rGPGCON = rGPGCON & (~((3<<22)|(3<<6))) | ((0<<22)|(0<<6)) ;		//GPG11,3 set input
	rGPFCON = rGPFCON & (~((3<<4)|(3<<0))) | ((0<<4)|(0<<0)) ;		//GPF2,0 set input
	
	if(rINTPND==BIT_EINT8_23) 
	{
		ClearPending(BIT_EINT8_23);
		if(rEINTPEND&(1<<11)) 
		{
			//puts("Interrupt eint11 occur...");
			rEINTPEND |= 1<< 11;
		}
		
		if(rEINTPEND&(1<<19)) 
		{
			//puts("Interrupt eint19 occur...");		
			rEINTPEND |= 1<< 19;
		}
	}
	
	else if(rINTPND==BIT_EINT0)
	{
		//puts("Interrupt eint0 occur...");
		ClearPending(BIT_EINT0);
	}
	
	else if(rINTPND==BIT_EINT2) 
	{
		//puts("Interrupt eint2 occur...");
		ClearPending(BIT_EINT2);
	}

	//查询按键键值
	key = Key_Scan() ;
	if( key != 0xff )
		printf( "Interrupt occur... K%d is pressed!\r\n", key ) ;

	//Beep( 2000, 3000 ) ;

	//重新初始化IO口
	rGPGCON = rGPGCON & (~((3<<12)|(3<<4))) | ((1<<12)|(1<<4)) ;		//GPG6,2 set output
	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2)));		//GPG6,2 output 0
	
	rGPECON = rGPECON & (~((3<<26)|(3<<22))) | ((1<<26)|(1<<22));		//GPE13,11 set output
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11)));		//GPE13,11 output 0
	
	rGPGCON = rGPGCON & (~((3<<22)|(3<<6))) | ((2<<22)|(2<<6)) ;		//GPG11,3 set EINT
	rGPFCON = rGPFCON & (~((3<<4)|(3<<0))) | ((2<<4)|(2<<0)) ;		//GPF2,0 set EINT
}

void KeyScanInit(void)
{
	rGPGCON = rGPGCON & (~((3<<12)|(3<<4))) | ((1<<12)|(1<<4)) ;		//GPG6,2 set output
	rGPGDAT = rGPGDAT & (~((1<<6)|(1<<2)));		//GPG6,2 output 0
	
	rGPECON = rGPECON & (~((3<<26)|(3<<22))) | ((1<<26)|(1<<22));		//GPE13,11 set output
	rGPEDAT = rGPEDAT & (~((1<<13)|(1<<11)));		//GPE13,11 output 0
	
	rGPGCON = rGPGCON & (~((3<<22)|(3<<6))) | ((2<<22)|(2<<6)) ;		//GPG11,3 set EINT
	rGPFCON = rGPFCON & (~((3<<4)|(3<<0))) | ((2<<4)|(2<<0)) ;		//GPF2,0 set EINT
	
	rEXTINT0 &= ~(7|(7<<8));	
	rEXTINT0 |= (2|(2<<8));	//set eint0,2 falling edge int
	rEXTINT1 &= ~(7<<12);
	rEXTINT1 |= (2<<12);	//set eint11 falling edge int
	rEXTINT2 &= ~(0xf<<12);
	rEXTINT2 |= (2<<12);	//set eint19 falling edge int

	rEINTPEND |= (1<<11)|(1<<19);		//clear eint 11,19
	rEINTMASK &= ~((1<<11)|(1<<19));	//enable eint11,19
	ClearPending(BIT_EINT0|BIT_EINT2|BIT_EINT8_23);
	pISR_EINT0 = pISR_EINT2 = pISR_EINT8_23 = (U32)KeyISR;
	EnableIrq(BIT_EINT0|BIT_EINT2|BIT_EINT8_23);	
}

void Key_Scan_Test( void )
{
	
	Uart_Printf( "\r\n8X2 KEY array TEST ( Interrupt MODE )\r\n" );
	Uart_Printf( "Press 'ESC' key to Exit this program !\r\n\r\n" );

	KeyScanInit() ;
    while( Uart_GetKey() != ESC_KEY ) ;
	rGPGCON = rGPGCON & (~((3<<22)|(3<<6))) | ((0<<22)|(0<<6)) ;		//GPG11,3 set input
	rGPFCON = rGPFCON & (~((3<<4)|(3<<0))) | ((0<<4)|(0<<0)) ;		//GPF2,0 set input
}