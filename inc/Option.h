/**************************************************************
 NAME: option.h
 DESC: To measuure the USB download speed, the WDT is used.
       To measure up to large time, The WDT interrupt is used.
 HISTORY:
 Feb.20.2002:Shin, On Pil: Programming start
 Mar.25.2002:purnnamu: S3C2400X profile.c is ported for S3C2410X.
 **************************************************************/
 
#ifndef __OPTION_H__
#define __OPTION_H__

//#define FCLK 135000000
//#define FCLK 202800000
#define FCLK 200000000
#define HCLK (FCLK/2)
#define PCLK (HCLK/2)
//#define PCLK (HCLK)
#define UCLK 48000000

// BUSWIDTH : 16,32
#define BUSWIDTH    (32)

//64MB
// 0x30000000 ~ 0x30ffffff : Download Area (16MB) Cacheable
// 0x31000000 ~ 0x33feffff : Non-Cacheable Area
// 0x33ff0000 ~ 0x33ff47ff : Heap & RW Area
// 0x33ff4800 ~ 0x33ff7fff : FIQ ~ User Stack Area
// 0x33ff8000 ~ 0x33fffeff : Not Useed Area
// 0x33ffff00 ~ 0x33ffffff : Exception & ISR Vector Table
#define _RAM_STARTADDRESS 		0x30000000
#define	SDRAM_BASE				0x30000000

#define SDRAM_SIZE64M

#ifdef	SDRAM_SIZE8M
#define	SDRAM_END				0x30800000
#endif
#ifdef	SDRAM_SIZE16M
#define	SDRAM_END				0x31000000
#endif
#ifdef	SDRAM_SIZE32M
#define	SDRAM_END				0x32000000
#endif
#ifdef	SDRAM_SIZE64M
#define	SDRAM_END				0x34000000
#endif

#define _ISR_STARTADDRESS 		(SDRAM_END-0x100)		//0x33ffff00     
#define _MMUTT_STARTADDRESS		(SDRAM_END-0x8000)		//0x33ff8000
#define _STACK_BASEADDRESS		(SDRAM_END-0x8000)		//0x33ff8000
#define HEAPEND		  			(SDRAM_END-0x10000)		//0x33ff0000
#define _NONCACHE_STARTADDRESS	(SDRAM_BASE+0x400000)	//0x30400000

//If you use ADS1.x, please define ADS10
#define ADS10 TRUE

//USB Device Options
#define USBDMA	TRUE
#define USBDMA_DEMAND	FALSE	//the downloadFileSize should be (64*n)
#define BULK_PKT_SIZE	32

#define		UART_BAUD		115200		//´®¿Ú²¨ÌØÂÊ

// note: makefile,option.a should be changed

#endif /*__OPTION_H__*/
