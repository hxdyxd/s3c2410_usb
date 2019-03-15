#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "rtcapi.h"
#include "alarm.h"
#include "timer.h"

static int AlarmWake;

static void AlarmNotify(void)
{
	AlarmWake = 1;
}

//================================================================================
//关闭所有IO口控制的电源,将不用的IO口设置为输入并禁止上拉
static void ConfigPowerOffGPIO(void)
{
	 //CAUTION:Follow the configuration order for setting the ports. 
    // 1) setting value(GPnDAT) 
    // 2) setting control register  (GPnCON)
    // 3) configure pull-up resistor(GPnUP)  

    //32bit data bus configuration  
    //*** PORT A GROUP
    //Ports  : GPA22 GPA21  GPA20 GPA19 GPA18 GPA17 GPA16 GPA15 GPA14 GPA13 GPA12  
    //Signal : nFCE nRSTOUT nFRE   nFWE  ALE   CLE  nGCS5 nGCS4 nGCS3 nGCS2 nGCS1 
    //Binary :  1     0      1  , 1   1   1    1   ,  1     1     1     1
    //Ports  : GPA11   GPA10  GPA9   GPA8   GPA7   GPA6   GPA5   GPA4   GPA3   GPA2   GPA1  GPA0
    //Signal : ADDR26 ADDR25 ADDR24 ADDR23 ADDR22 ADDR21 ADDR20 ADDR19 ADDR18 ADDR17 ADDR16 ADDR0 
    //Binary :  1       1      1      1   , 1       1      1      1   ,  1       1     1      0
    rGPACON = 0x7fffff;

    //**** PORT B GROUP
    //Ports  : GPB10    GPB9    GPB8    GPB7    GPB6     GPB5    GPB4   GPB3   GPB2     GPB1      GPB0
    //Signal : nXDREQ0 nXDACK0 nXDREQ1 nXDACK1 nSS_KBD nDIS_OFF L3CLOCK L3DATA L3MODE nIrDATXDEN Keyboard
    //Setting: INPUT  OUTPUT   INPUT  OUTPUT   INPUT   OUTPUT   OUTPUT OUTPUT OUTPUT   OUTPUT    OUTPUT 
    //Binary :   00  ,  01       00  ,   01      00   ,  01       01  ,   01     01   ,  01        01  
    rGPBCON = 0x0;
    rGPBUP  = 0x7ff;     // The pull up function is disabled GPB[10:0]

    //*** PORT C GROUP
    //Ports  : GPC15 GPC14 GPC13 GPC12 GPC11 GPC10 GPC9 GPC8  GPC7   GPC6   GPC5 GPC4 GPC3  GPC2  GPC1 GPC0
    //Signal : VD7   VD6   VD5   VD4   VD3   VD2   VD1  VD0 LCDVF2 LCDVF1 LCDVF0 VM VFRAME VLINE VCLK LEND  
    //Binary :  10   10  , 10    10  , 10    10  , 10   10  , 10     10  ,  10   10 , 10     10 , 10   10
    rGPCCON = 0x0;
    rGPCUP  = 0xffff;     // The pull up function is disabled GPC[15:0] 

    //*** PORT D GROUP
    //Ports  : GPD15 GPD14 GPD13 GPD12 GPD11 GPD10 GPD9 GPD8 GPD7 GPD6 GPD5 GPD4 GPD3 GPD2 GPD1 GPD0
    //Signal : VD23  VD22  VD21  VD20  VD19  VD18  VD17 VD16 VD15 VD14 VD13 VD12 VD11 VD10 VD9  VD8
    //Binary : 10    10  , 10    10  , 10    10  , 10   10 , 10   10 , 10   10 , 10   10 ,10   10
    rGPDCON = 0x0;
    rGPDUP  = 0xffff;     // The pull up function is disabled GPD[15:0]

    //*** PORT E GROUP
    //Ports  : GPE15  GPE14 GPE13   GPE12   GPE11   GPE10   GPE9    GPE8     GPE7  GPE6  GPE5   GPE4  
    //Signal : IICSDA IICSCL SPICLK SPIMOSI SPIMISO SDDATA3 SDDATA2 SDDATA1 SDDATA0 SDCMD SDCLK I2SSDO 
    //Binary :  10     10  ,  10      10  ,  10      10   ,  10      10   ,   10    10  , 10     10  ,     
    //-------------------------------------------------------------------------------------------------------
    //Ports  :  GPE3   GPE2  GPE1    GPE0    
    //Signal : I2SSDI CDCLK I2SSCLK I2SLRCK     
    //Binary :  10     10  ,  10      10 
    rGPECON = 0x0;
    rGPEUP  = 0xffff;     // The pull up function is disabled GPE[15:0]

    //*** PORT F GROUP
    //Ports  : GPF7   GPF6   GPF5   GPF4      GPF3     GPF2  GPF1   GPF0
    //Signal : nLED_8 nLED_4 nLED_2 nLED_1 nIRQ_PCMCIA EINT2 KBDINT EINT0
    //Setting: Output Output Output Output    EINT3    EINT2 EINT1  EINT0
    //Binary :  01      01 ,  01     01  ,     10       10  , 10     10
    rGPFCON = 0x0;
    rGPFUP  = 0xff;     // The pull up function is disabled GPF[7:0]

    //*** PORT G GROUP
    //Ports  : GPG15 GPG14 GPG13 GPG12 GPG11    GPG10    GPG9     GPG8     GPG7      GPG6    
    //Signal : nYPON  YMON nXPON XMON  EINT19 DMAMODE1 DMAMODE0 DMASTART KBDSPICLK KBDSPIMOSI
    //Setting: nYPON  YMON nXPON XMON  EINT19  Output   Output   Output   SPICLK1    SPIMOSI1
    //Binary :   11    11 , 11    11  , 10      01    ,   01       01   ,    11         11
    //-----------------------------------------------------------------------------------------
    //Ports  :    GPG5       GPG4    GPG3    GPG2    GPG1    GPG0    
    //Signal : KBDSPIMISO LCD_PWREN EINT11 nSS_SPI IRQ_LAN IRQ_PCMCIA
    //Setting:  SPIMISO1  LCD_PWRDN EINT11   nSS0   EINT9    EINT8
    //Binary :     11         11   ,  10      11  ,  10        10
    rGPGCON = 0x0;
    rGPGUP  = 0xffff;    // The pull up function is disabled GPG[15:0]

    //*** PORT H GROUP
    //Ports  :  GPH10    GPH9  GPH8 GPH7  GPH6  GPH5 GPH4 GPH3 GPH2 GPH1  GPH0 
    //Signal : CLKOUT1 CLKOUT0 UCLK nCTS1 nRTS1 RXD1 TXD1 RXD0 TXD0 nRTS0 nCTS0
    //Binary :   10   ,  10     10 , 11    11  , 10   10 , 10   10 , 10    10
    rGPHCON = 0x0;
    rGPHUP  = 0x7ff;    // The pull up function is disabled GPH[10:0]	
	
//	LcdBackLightOff();
}

void RtcTest(void)
{
	DATETIME_T dt1, dt2;
	DATE_T d1, d2;
	U32 i;
	U8 getalr, testmode;
	
	dt1.year   = 2004;
	dt1.month  = 5;
	dt1.day    = 31;
	dt1.hour   = 23;
	dt1.minute = 59;
	dt1.second = 54;
	
	dt2.year   = 2004;
	dt2.month  = 6;
	dt2.day    = 1;
	dt2.hour   = 0;
	dt2.minute = 0;
	dt2.second = 0;
	
	d1.year  = dt1.year;
	d1.month = dt1.month;
	d1.day   = dt1.day;
	d2.year  = dt2.year;
	d2.month = dt2.month;
	d2.day   = dt2.day;
	
//	printf("%d\r\n", RtcCmpDateTime(&dt2, &dt1));	
//	printf("%d\r\n", RtcGetDateInterval(&d1, &d2));
//	printf("%d\r\n", RtcGetTimeInterval(&dt1, &dt2));
	
/*	i = 36524;	
	printf("%d-%d-%d + %d = ", d2.year, d2.month, d2.day, i);
	RtcDateAddDays(&d2, i);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);
	printf("%d-%d-%d - %d = ", d2.year, d2.month, d2.day, i);
	RtcDateSubDays(&d2, i);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);
	
	printf("%d-%d-%d next month = ", d2.year, d2.month, d2.day);	
	RtcGetNextMonth(&d2);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);
	printf("%d-%d-%d pre month = ", d2.year, d2.month, d2.day);	
	RtcGetPreMonth(&d2);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);
	
	printf("%d-%d-%d next year = ", d2.year, d2.month, d2.day);	
	RtcGetNextYear(&d2);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);
	printf("%d-%d-%d pre year = ", d2.year, d2.month, d2.day);	
	RtcGetPreYear(&d2);
	printf("%d-%d-%d\r\n", d2.year, d2.month, d2.day);*/
	
	EnableModuleClock(CLOCK_RTC);
	
	puts("Please select which mode to test RTC alarm wake up\r\n");
	puts("1. Idle mode\r\n");
	puts("2. Power off mode\r\n");
	
	while(1) {
		U8 sel = getch();
		if((sel=='1')||(sel=='2')) {
			testmode = sel-'1';
			break;
		}
	}
		
	printf("Set current date %4d-%2d-%2d week %d, time %2d:%2d:%2d\r\n",
			dt1.year, dt1.month, dt1.day, dt1.week, dt1.hour, dt1.minute, dt1.second);
	RtcSetDate(&dt1);	
	
	if(testmode==1)	{		//enter power off mode
		U32 r;
		
		Uart_Printf("Enter Power-off mode, press Reset or K4 key to wake up\r\n");
		Uart_TxEmpty(0);	//Wait until UART0 Tx buffer empty.
		
		OpenAlarm(AlarmNotify);
		AlarmSetDate(&dt2);
		
		ConfigPowerOffGPIO();
		
		rGPGCON &= ~((3<<12)|(3<<4)|(3<<22)|(3<<6));
		rGPGCON |= 2<<6;	//set GPG3 as eint11
		rGPGUP  |= (1<<6)|(1<<2)|(1<<3);	//GPG6,2,3 input, pull-up disable
		rGPGUP  &= ~(1<<3);					//GPG3 input, pull-up enable
	
		rGPECON &= ~((3<<26)|(3<<22));
		rGPECON |= (1<<22);		//GPE11 output
		rGPEUP  |= 1<<13;		//GPE13 input pull-up disable
		rGPEDAT &= ~(1<<13);	//GPE11 output 0
	
		rGPFCON &= ~((3<<4)|3);
		rGPFUP  |= (1<<2)|1;				

		rEINTPEND = 1<<11;					//clear EINT11
		rEXTINT1  = 0;						//EINT11 low-level interrupt
    	rEINTMASK = rEINTMASK&~(1<<11); 	//SRCPND:EINT8_23 will be set by EINT19 after wake-up.
		ClearPending(BIT_EINT8_23|BIT_RTC);
		rINTMSK = BIT_ALLMSK;
		// NOTE: Any interrupt can't be used in STOP mode
		// because SDRAM is in self-refresh mode and ISR code will access SDRAM.

		rRTCCON = 0x0;		//R/W disable, 1/32768, Normal(merge), No reset
		rADCCON|=(1<<2);	//ADC additional power down

		rGSTATUS3=(U32)StartPointAfterPowerOffWakeUp;
		rGSTATUS4=0xaaaaaaaa;
		
		//MISCCR[13:12] set usb port0,1 suspend
		//MISCCR[2] Previous state at STOP(?) mode (???)
		//MISCCR[1:0] D0~D31 pull-up disable	
		rMISCCR |= 0x3007;		

		rLCDCON1 &= ~1;		//Before entering STOP mode, LCD must be off		

	//=================================================================
	//  VERY IMPORTANT NOTE
	//  To enter STOP/SLIDLE mode, MMU TLB-fill operation should be prohibited
	//  because MTT table is in SDRAM and SDRAM is in the self-refresh mode.
	//  So, we will fill TLB before entering SDRAM self-refresh
	//  instead of disabling MMU.
    	r = rREFRESH;	//To fill TLB for the special register used in EnterPWDN
    	r = rCLKCON;
	//=================================================================
	{
//		void (*pwrdn)(U32) = (void (*)(U32))0x48;	//EnterPWDN
//		(*pwrdn)(0x7fff8);
		rCLKCON = 0x7fff8;
	}
		EnterPWDN(0x7fff8);		//POWER_OFF mode
		//Never return here.
	}
		
	puts("Now set alarm and enter idle mode\r\n");
	
	OpenAlarm(AlarmNotify);
	
	AlarmSetDate(&dt2);
	getalr = AlarmGetDate(&dt1);

	AlarmWake = 0;
	rCLKCON	|= 1<<2;		//enter IDLE mode
	
	for(i=0; i<10; i++);
	
	rCLKCON &= ~(1<<2);		//manual clear
	
	CloseAlarm();

	printf("CPU wake up from idle mode by alarm %s\r\n", AlarmWake?"success":"fail");
	
	if(getalr)
		printf("Alarm time = %4d-%2d-%2d %2d:%2d:%2d\r\n", dt1.year, dt1.month, dt1.day, dt1.hour, dt1.minute, dt1.second);
	else
		puts("Get alarm time fail\r\n");
	
	puts("Real time clock display, press Esc key to exit\r\n");
	
	while(1) {
		if(getkey()==0x1b)
			break;
		RtcGetDate(&dt1);
		printf("Now date is %4d-%2d-%2d week %d, time is %2d:%2d:%2d",
				dt1.year, dt1.month, dt1.day, dt1.week, dt1.hour, dt1.minute, dt1.second);
		puts("\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b\b");
	}
}


//==================================================================================		
void RTC_Time_Set( void )
{
	rRTCCON = 1 ;		//RTC read and write enable

	rBCDYEAR = 0x05 ;		//年
    rBCDMON  = 0x06 ;		//月
    rBCDDATE = 0x19 ;		//日	
	rBCDDAY  = 0x02 ;		//星期
	rBCDHOUR = 0x15 ;		//小时
    rBCDMIN  = 0x21 ;		//分
    rBCDSEC  = 0x30 ;		//秒
	
	rRTCCON &= ~1 ;		//RTC read and write disable
}

//==================================================================================
void RTC_Display(void) 
{
	U16 year ;
	U8 month, day ;		// week
	U8 hour, minute, second ;

	RTC_Time_Set() ;
	   
   	Uart_Printf( "\r\nRTC TIME Display, press ESC key to exit !\r\n" ) ;

    while( Uart_GetKey() != ESC_KEY )
    {
		rRTCCON = 1 ;		//RTC read and write enable

		year = 0x2000+rBCDYEAR  ;		//年
	    month = rBCDMON  ;		//月
	    day = rBCDDATE  ;		//日	
//		week = rBCDDAY  ;		//星期
		hour = rBCDHOUR  ;		//小时
	    minute = rBCDMIN  ;		//分
	    second = rBCDSEC  ;		//秒
		
		rRTCCON &= ~1 ;		//RTC read and write disable

	    Uart_Printf( "RTC time : %04x-%02x-%02x %02x:%02x:%02x\r\n", year, month, day, hour, minute, second );
		Delay( 9000 ) ;
    }
}
