
//=============================================================================
// File Name : s3c2410.h
// Function  : S3C2410 Define Address Register
// Program   : Shin, On Pil (SOP)
// Date      : May 06, 2002
// Version   : 0.0
// History
//   0.0 : Programming start (February 15,2002) -> SOP
//         INTERRUPT rPRIORITY 0x4a00000a -> 0x4a00000c       (May 02, 2002 SOP)
//         RTC BCD DAY and DATE Register Name Correction      (May 06, 2002 SOP) 
//=============================================================================
#ifndef __S3C2410_H__
#define __S3C2410_H__
#include"2410addr.h" 
#ifdef __cplusplus
extern "C" 
{
 #endif
    /* data type definitions */
    #ifndef U32
    #define U32 unsigned long
    #endif
    #ifndef S32
    #define S32 signed long
    #endif
    #ifndef I32
    #define I32 long
    #endif
    #ifndef U16
    #define U16 unsigned short
    #endif
    #ifndef S16
    #define S16 signed short
    #endif
    #ifndef I16
    #define I16 short
    #endif
    #ifndef U8
    #define U8  unsigned char
    #endif
    #ifndef S8
    #define S8  signed char
    #endif
    #ifndef I8
    #define I8  char
    #endif
    #ifndef __u8
    #define __u8 unsigned char
    #endif
    #ifndef __u16
    #define __u16 unsigned short
    #endif
    #ifndef __u32
    #define __u32 unsigned long
    #endif
    #ifndef NULL
    #define NULL	0
    #endif
    typedef volatile U8    S3C24X0_REG8;
    typedef volatile U16   S3C24X0_REG16;
    typedef volatile U32   S3C24X0_REG32;
    typedef volatile unsigned long	vu_long;
    typedef volatile unsigned short    vu_short;
    typedef volatile unsigned char	vu_char;
    #ifndef u_char
    #define  u_char  unsigned char
    #endif
    #ifndef u_short
    #define   u_short unsigned short
    #endif
    #ifndef u_int
    #define   u_int unsigned int
    #endif
    #ifndef u_long
    #define   u_long unsigned long
    #endif
    #ifndef uchar
    #define   uchar unsigned char
    #endif
    #ifndef ushort
    #define   ushort unsigned short
    #endif
    #ifndef uint
    #define   uint unsigned int
    #endif
    #ifndef ulong
    #define   ulong unsigned long	
    #endif
    /* Memory controller (see manual chapter 5) */
    __packed
		typedef struct 
    {
        S3C24X0_REG32	BWSCON;
        S3C24X0_REG32	BANKCON[8];
        S3C24X0_REG32	REFRESH;
        S3C24X0_REG32	BANKSIZE;
        S3C24X0_REG32	MRSRB6;
        S3C24X0_REG32	MRSRB7;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_MEMCTL;
    /* USB HOST (see manual chapter 12) */
    __packed 
		typedef struct 
    {
        S3C24X0_REG32	HcRevision;
        S3C24X0_REG32	HcControl;
        S3C24X0_REG32	HcCommonStatus;
        S3C24X0_REG32	HcInterruptStatus;
        S3C24X0_REG32	HcInterruptEnable;
        S3C24X0_REG32	HcInterruptDisable;
        S3C24X0_REG32	HcHCCA;
        S3C24X0_REG32	HcPeriodCuttendED;
        S3C24X0_REG32	HcControlHeadED;
        S3C24X0_REG32	HcControlCurrentED;
        S3C24X0_REG32	HcBulkHeadED;
        S3C24X0_REG32	HcBuldCurrentED;
        S3C24X0_REG32	HcDoneHead;
        S3C24X0_REG32	HcRmInterval;
        S3C24X0_REG32	HcFmRemaining;
        S3C24X0_REG32	HcFmNumber;
        S3C24X0_REG32	HcPeriodicStart;
        S3C24X0_REG32	HcLSThreshold;
        S3C24X0_REG32	HcRhDescriptorA;
        S3C24X0_REG32	HcRhDescriptorB;
        S3C24X0_REG32	HcRhStatus;
        S3C24X0_REG32	HcRhPortStatus1;
        S3C24X0_REG32	HcRhPortStatus2;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_USB_HOST;
    /* INTERRUPT (see manual chapter 14) */
    __packed 
		typedef struct 
    {
        S3C24X0_REG32	SRCPND;
        S3C24X0_REG32	INTMOD;
        S3C24X0_REG32	INTMSK;
        S3C24X0_REG32	PRIORITY;
        S3C24X0_REG32	INTPND;
        S3C24X0_REG32	INTOFFSET;
        S3C24X0_REG32	SUBSRCPND;
        S3C24X0_REG32	INTSUBMSK;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_INTERRUPT;
    /* DMAS (see manual chapter 8) */
    __packed
		typedef struct 
    {
        S3C24X0_REG32	DISRC;
        S3C24X0_REG32	DISRCC;
        S3C24X0_REG32	DIDST;
        S3C24X0_REG32	DIDSTC;
        S3C24X0_REG32	DCON;
        S3C24X0_REG32	DSTAT;
        S3C24X0_REG32	DCSRC;
        S3C24X0_REG32	DCDST;
        S3C24X0_REG32	DMASKTRIG;
        S3C24X0_REG32	res[7];
    } 
    /*__attribute__((__packed__))*/ S3C24X0_DMA;
    __packed
		typedef struct 
    {
        S3C24X0_DMA	dma[4];
    } 
    /*__attribute__((__packed__))*/ S3C24X0_DMAS;
    /* CLOCK & POWER MANAGEMENT (see S3C2400 manual chapter 6) */
    /*                          (see S3C2410 manual chapter 7) */
    __packed
		typedef struct 
    {
        S3C24X0_REG32	LOCKTIME;
        S3C24X0_REG32	MPLLCON;
        S3C24X0_REG32	UPLLCON;
        S3C24X0_REG32	CLKCON;
        S3C24X0_REG32	CLKSLOW;
        S3C24X0_REG32	CLKDIVN;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_CLOCK_POWER;
    /* LCD CONTROLLER (see manual chapter 15) */
//    __packed
		typedef struct 
    {
        S3C24X0_REG32	LCDCON1;
        S3C24X0_REG32	LCDCON2;
        S3C24X0_REG32	LCDCON3;
        S3C24X0_REG32	LCDCON4;
        S3C24X0_REG32	LCDCON5;
        S3C24X0_REG32	LCDSADDR1;
        S3C24X0_REG32	LCDSADDR2;
        S3C24X0_REG32	LCDSADDR3;
        S3C24X0_REG32	REDLUT;
        S3C24X0_REG32	GREENLUT;
        S3C24X0_REG32	BLUELUT;
        S3C24X0_REG32	res[8];
        S3C24X0_REG32	DITHMODE;
        S3C24X0_REG32	TPAL;
        S3C24X0_REG32	LCDINTPND;
        S3C24X0_REG32	LCDSRCPND;
        S3C24X0_REG32	LCDINTMSK;
        S3C24X0_REG32	LPCSEL;
    } 
    /*__attribute__((__packed__))*/
	S3C24X0_LCD;
    /* NAND FLASH (see S3C2410 manual chapter 6) */
    /*
    typedef struct 
    {
        S3C24X0_REG32	NFCONF;
        S3C24X0_REG32	NFCMD;
        S3C24X0_REG32	NFADDR;
        S3C24X0_REG32	NFDATA;
        S3C24X0_REG32	NFSTAT;
        S3C24X0_REG32	NFECC;
    }
	*/
    /*__attribute__((__packed__))*/
	//S3C2410_NAND;
    /* UART (see manual chapter 11) */
//    __packed
		typedef struct 
    {
        S3C24X0_REG32	ULCON;
        S3C24X0_REG32	UCON;
        S3C24X0_REG32	UFCON;
        S3C24X0_REG32	UMCON;
        S3C24X0_REG32	UTRSTAT;
        S3C24X0_REG32	UERSTAT;
        S3C24X0_REG32	UFSTAT;
        S3C24X0_REG32	UMSTAT;
        #ifdef __BIG_ENDIAN
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	UTXH;
        S3C24X0_REG8	res2[3];
        S3C24X0_REG8	URXH;
        #else /* Little Endian */
        S3C24X0_REG8	UTXH;
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	URXH;
        S3C24X0_REG8	res2[3];
        #endif
        S3C24X0_REG32	UBRDIV;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_UART;
    /* PWM TIMER (see manual chapter 10) */
//   __packed
	   typedef struct 
    {
        S3C24X0_REG32	TCNTB;
        S3C24X0_REG32	TCMPB;
        S3C24X0_REG32	TCNTO;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_TIMER;
//    __packed
		typedef struct 
    {
        S3C24X0_REG32	TCFG0;
        S3C24X0_REG32	TCFG1;
        S3C24X0_REG32	TCON;
        S3C24X0_TIMER	ch[4];
        S3C24X0_REG32	TCNTB4;
        S3C24X0_REG32	TCNTO4;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_TIMERS;
    /* USB DEVICE (see manual chapter 13) */
//   __packed
	   typedef struct 
    {
        #ifdef __BIG_ENDIAN
        S3C24X0_REG8	res[3];
        S3C24X0_REG8	EP_FIFO_REG;
        #else /*  little endian */
        S3C24X0_REG8	EP_FIFO_REG;
        S3C24X0_REG8	res[3];
        #endif
    } 
    /*__attribute__((__packed__))*/ S3C24X0_USB_DEV_FIFOS;
//    __packed
		typedef struct 
    {
        #ifdef __BIG_ENDIAN
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	EP_DMA_CON;
        S3C24X0_REG8	res2[3];
        S3C24X0_REG8	EP_DMA_UNIT;
        S3C24X0_REG8	res3[3];
        S3C24X0_REG8	EP_DMA_FIFO;
        S3C24X0_REG8	res4[3];
        S3C24X0_REG8	EP_DMA_TTC_L;
        S3C24X0_REG8	res5[3];
        S3C24X0_REG8	EP_DMA_TTC_M;
        S3C24X0_REG8	res6[3];
        S3C24X0_REG8	EP_DMA_TTC_H;
        #else /*  little endian */
        S3C24X0_REG8	EP_DMA_CON;
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	EP_DMA_UNIT;
        S3C24X0_REG8	res2[3];
        S3C24X0_REG8	EP_DMA_FIFO;
        S3C24X0_REG8	res3[3];
        S3C24X0_REG8	EP_DMA_TTC_L;
        S3C24X0_REG8	res4[3];
        S3C24X0_REG8	EP_DMA_TTC_M;
        S3C24X0_REG8	res5[3];
        S3C24X0_REG8	EP_DMA_TTC_H;
        S3C24X0_REG8	res6[3];
        #endif
    } 
    /*__attribute__((__packed__))*/ S3C24X0_USB_DEV_DMAS;

	typedef /*__packed */ struct
    {
        #ifdef __BIG_ENDIAN
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	FUNC_ADDR_REG;
        S3C24X0_REG8	res2[3];
        S3C24X0_REG8	PWR_REG;
        S3C24X0_REG8	res3[3];
        S3C24X0_REG8	EP_INT_REG;
        S3C24X0_REG8	res4[15];
        S3C24X0_REG8	USB_INT_REG;
        S3C24X0_REG8	res5[3];
        S3C24X0_REG8	EP_INT_EN_REG;
        S3C24X0_REG8	res6[15];
        S3C24X0_REG8	USB_INT_EN_REG;
        S3C24X0_REG8	res7[3];
        S3C24X0_REG8	FRAME_NUM1_REG;
        S3C24X0_REG8	res8[3];
        S3C24X0_REG8	FRAME_NUM2_REG;
        S3C24X0_REG8	res9[3];
        S3C24X0_REG8	INDEX_REG;
        S3C24X0_REG8	res10[7];
        S3C24X0_REG8	MAXP_REG;
        S3C24X0_REG8	res11[3];
        S3C24X0_REG8	EP0_CSR_IN_CSR1_REG;
        S3C24X0_REG8	res12[3];
        S3C24X0_REG8	IN_CSR2_REG;
        S3C24X0_REG8	res13[7];
        S3C24X0_REG8	OUT_CSR1_REG;
        S3C24X0_REG8	res14[3];
        S3C24X0_REG8	OUT_CSR2_REG;
        S3C24X0_REG8	res15[3];
        S3C24X0_REG8	OUT_FIFO_CNT1_REG;
        S3C24X0_REG8	res16[3];
        S3C24X0_REG8	OUT_FIFO_CNT2_REG;
        #else /*  little endian */
        S3C24X0_REG8	FUNC_ADDR_REG;
        S3C24X0_REG8	res1[3];
        S3C24X0_REG8	PWR_REG;
        S3C24X0_REG8	res2[3];
        S3C24X0_REG8	EP_INT_REG;
        S3C24X0_REG8	res3[15];
        S3C24X0_REG8	USB_INT_REG;
        S3C24X0_REG8	res4[3];
        S3C24X0_REG8	EP_INT_EN_REG;
        S3C24X0_REG8	res5[15];
        S3C24X0_REG8	USB_INT_EN_REG;
        S3C24X0_REG8	res6[3];
        S3C24X0_REG8	FRAME_NUM1_REG;
        S3C24X0_REG8	res7[3];
        S3C24X0_REG8	FRAME_NUM2_REG;
        S3C24X0_REG8	res8[3];
        S3C24X0_REG8	INDEX_REG;
        S3C24X0_REG8	res9[7];
        S3C24X0_REG8	MAXP_REG;
        S3C24X0_REG8	res10[7];
        S3C24X0_REG8	EP0_CSR_IN_CSR1_REG;
        S3C24X0_REG8	res11[3];
        S3C24X0_REG8	IN_CSR2_REG;
        S3C24X0_REG8	res12[3];
        S3C24X0_REG8	OUT_CSR1_REG;
        S3C24X0_REG8	res13[7];
        S3C24X0_REG8	OUT_CSR2_REG;
        S3C24X0_REG8	res14[3];
        S3C24X0_REG8	OUT_FIFO_CNT1_REG;
        S3C24X0_REG8	res15[3];
        S3C24X0_REG8	OUT_FIFO_CNT2_REG;
        S3C24X0_REG8	res16[3];
        #endif /*  __BIG_ENDIAN */
        S3C24X0_USB_DEV_FIFOS	fifo[5];
        S3C24X0_USB_DEV_DMAS	dma[5];
    } 
    /*__attribute__((__packed__))*/ S3C24X0_USB_DEVICE;
    /* WATCH DOG TIMER (see manual chapter 18) */
  //  __packed
	typedef struct 
    {
        S3C24X0_REG32	WTCON;
        S3C24X0_REG32	WTDAT;
        S3C24X0_REG32	WTCNT;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_WATCHDOG;
    /* IIC (see manual chapter 20) */
    typedef struct 
    {
        S3C24X0_REG32	IICCON;
        S3C24X0_REG32	IICSTAT;
        S3C24X0_REG32	IICADD;
        S3C24X0_REG32	IICDS;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_I2C;
    /* IIS (see manual chapter 21) */
/*
	typedef struct 
    {
        #ifdef __BIG_ENDIAN
        S3C24X0_REG16	res1;
        S3C24X0_REG16	IISCON;
        S3C24X0_REG16	res2;
        S3C24X0_REG16	IISMOD;
        S3C24X0_REG16	res3;
        S3C24X0_REG16	IISPSR;
        S3C24X0_REG16	res4;
        S3C24X0_REG16	IISFCON;
        S3C24X0_REG16	res5;
        S3C24X0_REG16	IISFIFO;
        #else 
        S3C24X0_REG16	IISCON;
        S3C24X0_REG16	res1;
        S3C24X0_REG16	IISMOD;
        S3C24X0_REG16	res2;
        S3C24X0_REG16	IISPSR;
        S3C24X0_REG16	res3;
        S3C24X0_REG16	IISFCON;
        S3C24X0_REG16	res4;
        S3C24X0_REG16	IISFIFO;
        S3C24X0_REG16	res5;
        #endif
    } 
	*/
    /*__attribute__((__packed__))*/ 
	//S3C24X0_I2S;
    /* I/O PORT (see manual chapter 9) */
    __packed
	typedef struct
    {
        S3C24X0_REG32	GPACON;
        S3C24X0_REG32	GPADAT;
        S3C24X0_REG32	res1[2];
        S3C24X0_REG32	GPBCON;
        S3C24X0_REG32	GPBDAT;
        S3C24X0_REG32	GPBUP;
        S3C24X0_REG32	res2;
        S3C24X0_REG32	GPCCON;
        S3C24X0_REG32	GPCDAT;
        S3C24X0_REG32	GPCUP;
        S3C24X0_REG32	res3;
        S3C24X0_REG32	GPDCON;
        S3C24X0_REG32	GPDDAT;
        S3C24X0_REG32	GPDUP;
        S3C24X0_REG32	res4;
        S3C24X0_REG32	GPECON;
        S3C24X0_REG32	GPEDAT;
        S3C24X0_REG32	GPEUP;
        S3C24X0_REG32	res5;
        S3C24X0_REG32	GPFCON;
        S3C24X0_REG32	GPFDAT;
        S3C24X0_REG32	GPFUP;
        S3C24X0_REG32	res6;
        S3C24X0_REG32	GPGCON;
        S3C24X0_REG32	GPGDAT;
        S3C24X0_REG32	GPGUP;
        S3C24X0_REG32	res7;
        S3C24X0_REG32	GPHCON;
        S3C24X0_REG32	GPHDAT;
        S3C24X0_REG32	GPHUP;
        S3C24X0_REG32	res8;
        S3C24X0_REG32	MISCCR;
        S3C24X0_REG32	DCLKCON;
        S3C24X0_REG32	EXTINT0;
        S3C24X0_REG32	EXTINT1;
        S3C24X0_REG32	EXTINT2;
        S3C24X0_REG32	EINTFLT0;
        S3C24X0_REG32	EINTFLT1;
        S3C24X0_REG32	EINTFLT2;
        S3C24X0_REG32	EINTFLT3;
        S3C24X0_REG32	EINTMASK;
        S3C24X0_REG32	EINTPEND;
        S3C24X0_REG32	GSTATUS0;
        S3C24X0_REG32	GSTATUS1;
        S3C24X0_REG32	GSTATUS2;
        S3C24X0_REG32	GSTATUS3;
        S3C24X0_REG32	GSTATUS4;
    } 
    /*__attribute__((__packed__))*/ S3C24X0_GPIO;
    /* S3C2410 device base addresses */
    #define S3C24X0_MEMCTL_BASE		0x48000000
    #define S3C24X0_USB_HOST_BASE		0x49000000
    #define S3C24X0_INTERRUPT_BASE	0x4A000000
    #define S3C24X0_DMA_BASE			0x4B000000
    #define S3C24X0_CLOCK_POWER_BASE	0x4C000000
    #define S3C24X0_LCD_BASE			0x4D000000
    #define S3C2410_NAND_BASE			0x4E000000

    #define S3C24X0_UART_BASE			0x50000000
    #define S3C24X0_TIMER_BASE			0x51000000
    #define S3C24X0_USB_DEVICE_BASE	0x52000140
    #define S3C24X0_WATCHDOG_BASE	0x53000000
    #define S3C24X0_I2C_BASE			0x54000000
    #define S3C24X0_I2S_BASE			0x55000000
    #define S3C24X0_GPIO_BASE			0x56000000
    #define S3C24X0_RTC_BASE			0x57000000
    #define S3C2410_ADC_BASE			0x58000000
    #define S3C24X0_SPI_BASE			0x59000000
    #define S3C2410_SDI_BASE			0x5A000000

	

    static __inline S3C24X0_MEMCTL * const S3C24X0_GetBase_MEMCTL(void)
    {
        return (S3C24X0_MEMCTL * const)S3C24X0_MEMCTL_BASE;
    }
    static __inline S3C24X0_USB_HOST * const S3C24X0_GetBase_USB_HOST(void)
    {
        return (S3C24X0_USB_HOST * const)S3C24X0_USB_HOST_BASE;
    }
    static __inline S3C24X0_INTERRUPT * const S3C24X0_GetBase_INTERRUPT(void)
    {
        return (S3C24X0_INTERRUPT * const)S3C24X0_INTERRUPT_BASE;
    }
    static __inline S3C24X0_DMAS * const S3C24X0_GetBase_DMAS(void)
    {
        return (S3C24X0_DMAS * const)S3C24X0_DMA_BASE;
    }
    static __inline S3C24X0_CLOCK_POWER * const S3C24X0_GetBase_CLOCK_POWER(void)
    {
        return (S3C24X0_CLOCK_POWER * const)S3C24X0_CLOCK_POWER_BASE;
    }
    static __inline S3C24X0_TIMERS * const S3C24X0_GetBase_TIMERS(void)
    {
        return (S3C24X0_TIMERS * const)S3C24X0_TIMER_BASE;
    }
    static __inline S3C24X0_USB_DEVICE * const S3C24X0_GetBase_USB_DEVICE(void)
    {
        return (S3C24X0_USB_DEVICE * const)S3C24X0_USB_DEVICE_BASE;
    }
    static __inline S3C24X0_WATCHDOG * const S3C24X0_GetBase_WATCHDOG(void)
    {
        return (S3C24X0_WATCHDOG * const)S3C24X0_WATCHDOG_BASE;
    }
    static __inline S3C24X0_GPIO * const S3C24X0_GetBase_GPIO(void)
    {
        return (S3C24X0_GPIO * const)S3C24X0_GPIO_BASE;
    }
/*
	// Memory control 
    #define rBWSCON    (*(volatile unsigned *)0x48000000) //Bus width & wait status
    #define rBANKCON0  (*(volatile unsigned *)0x48000004) //Boot ROM control
    #define rBANKCON1  (*(volatile unsigned *)0x48000008) //BANK1 control
    #define rBANKCON2  (*(volatile unsigned *)0x4800000c) //BANK2 cControl
    #define rBANKCON3  (*(volatile unsigned *)0x48000010) //BANK3 control
    #define rBANKCON4  (*(volatile unsigned *)0x48000014) //BANK4 control
    #define rBANKCON5  (*(volatile unsigned *)0x48000018) //BANK5 control
    #define rBANKCON6  (*(volatile unsigned *)0x4800001c) //BANK6 control
    #define rBANKCON7  (*(volatile unsigned *)0x48000020) //BANK7 control
    #define rREFRESH   (*(volatile unsigned *)0x48000024) //DRAM/SDRAM refresh
    #define rBANKSIZE  (*(volatile unsigned *)0x48000028) //Flexible Bank Size
    #define rMRSRB6    (*(volatile unsigned *)0x4800002c) //Mode register set for SDRAM
    #define rMRSRB7    (*(volatile unsigned *)0x48000030) //Mode register set for SDRAM
    // USB HOST
    #define rHcRevision		(*(volatile unsigned *)0x49000000)
    #define rHcControl		(*(volatile unsigned *)0x49000004)
    #define rHcCommonStatus		(*(volatile unsigned *)0x49000008)
    #define rHcInterruptStatus	(*(volatile unsigned *)0x4900000C)
    #define rHcInterruptEnable	(*(volatile unsigned *)0x49000010)
    #define rHcInterruptDisable	(*(volatile unsigned *)0x49000014)
    #define rHcHCCA			(*(volatile unsigned *)0x49000018)
    #define rHcPeriodCuttendED	(*(volatile unsigned *)0x4900001C)
    #define rHcControlHeadED	(*(volatile unsigned *)0x49000020)
    #define rHcControlCurrentED	(*(volatile unsigned *)0x49000024)
    #define rHcBulkHeadED		(*(volatile unsigned *)0x49000028)
    #define rHcBuldCurrentED	(*(volatile unsigned *)0x4900002C)
    #define rHcDoneHead		(*(volatile unsigned *)0x49000030)
    #define rHcRmInterval		(*(volatile unsigned *)0x49000034)
    #define rHcFmRemaining		(*(volatile unsigned *)0x49000038)
    #define rHcFmNumber		(*(volatile unsigned *)0x4900003C)
    #define rHcPeriodicStart	(*(volatile unsigned *)0x49000040)
    #define rHcLSThreshold		(*(volatile unsigned *)0x49000044)
    #define rHcRhDescriptorA	(*(volatile unsigned *)0x49000048)
    #define rHcRhDescriptorB	(*(volatile unsigned *)0x4900004C)
    #define rHcRhStatus		(*(volatile unsigned *)0x49000050)
    #define rHcRhPortStatus1	(*(volatile unsigned *)0x49000054)
    #define rHcRhPortStatus2	(*(volatile unsigned *)0x49000058)
    // INTERRUPT
    #define rSRCPND     (*(volatile unsigned *)0x4a000000) //Interrupt request status
    #define rINTMOD     (*(volatile unsigned *)0x4a000004) //Interrupt mode control
    #define rINTMSK     (*(volatile unsigned *)0x4a000008) //Interrupt mask control
    #define rPRIORITY   (*(volatile unsigned *)0x4a00000c) //IRQ priority control
    #define rINTPND     (*(volatile unsigned *)0x4a000010) //Interrupt request status
    #define rINTOFFSET  (*(volatile unsigned *)0x4a000014) //Interruot request source offset
    #define rSUBSRCPND  (*(volatile unsigned *)0x4a000018) //Sub source pending
    #define rINTSUBMSK  (*(volatile unsigned *)0x4a00001c) //Interrupt sub mask
    // DMA
    #define rDISRC0     (*(volatile unsigned *)0x4b000000) //DMA 0 Initial source
    #define rDISRCC0    (*(volatile unsigned *)0x4b000004) //DMA 0 Initial source control
    #define rDIDST0     (*(volatile unsigned *)0x4b000008) //DMA 0 Initial Destination
    #define rDIDSTC0    (*(volatile unsigned *)0x4b00000c) //DMA 0 Initial Destination control
    #define rDCON0      (*(volatile unsigned *)0x4b000010) //DMA 0 Control
    #define rDSTAT0     (*(volatile unsigned *)0x4b000014) //DMA 0 Status
    #define rDCSRC0     (*(volatile unsigned *)0x4b000018) //DMA 0 Current source
    #define rDCDST0     (*(volatile unsigned *)0x4b00001c) //DMA 0 Current destination
    #define rDMASKTRIG0 (*(volatile unsigned *)0x4b000020) //DMA 0 Mask trigger
    #define rDISRC1     (*(volatile unsigned *)0x4b000040) //DMA 1 Initial source
    #define rDISRCC1    (*(volatile unsigned *)0x4b000044) //DMA 1 Initial source control
    #define rDIDST1     (*(volatile unsigned *)0x4b000048) //DMA 1 Initial Destination
    #define rDIDSTC1    (*(volatile unsigned *)0x4b00004c) //DMA 1 Initial Destination control
    #define rDCON1      (*(volatile unsigned *)0x4b000050) //DMA 1 Control
    #define rDSTAT1     (*(volatile unsigned *)0x4b000054) //DMA 1 Status
    #define rDCSRC1     (*(volatile unsigned *)0x4b000058) //DMA 1 Current source
    #define rDCDST1     (*(volatile unsigned *)0x4b00005c) //DMA 1 Current destination
    #define rDMASKTRIG1 (*(volatile unsigned *)0x4b000060) //DMA 1 Mask trigger
    #define rDISRC2     (*(volatile unsigned *)0x4b000080) //DMA 2 Initial source
    #define rDISRCC2    (*(volatile unsigned *)0x4b000084) //DMA 2 Initial source control
    #define rDIDST2     (*(volatile unsigned *)0x4b000088) //DMA 2 Initial Destination
    #define rDIDSTC2    (*(volatile unsigned *)0x4b00008c) //DMA 2 Initial Destination control
    #define rDCON2      (*(volatile unsigned *)0x4b000090) //DMA 2 Control
    #define rDSTAT2     (*(volatile unsigned *)0x4b000094) //DMA 2 Status
    #define rDCSRC2     (*(volatile unsigned *)0x4b000098) //DMA 2 Current source
    #define rDCDST2     (*(volatile unsigned *)0x4b00009c) //DMA 2 Current destination
    #define rDMASKTRIG2 (*(volatile unsigned *)0x4b0000a0) //DMA 2 Mask trigger
    #define rDISRC3     (*(volatile unsigned *)0x4b0000c0) //DMA 3 Initial source
    #define rDISRCC3    (*(volatile unsigned *)0x4b0000c4) //DMA 3 Initial source control
    #define rDIDST3     (*(volatile unsigned *)0x4b0000c8) //DMA 3 Initial Destination
    #define rDIDSTC3    (*(volatile unsigned *)0x4b0000cc) //DMA 3 Initial Destination control
    #define rDCON3      (*(volatile unsigned *)0x4b0000d0) //DMA 3 Control
    #define rDSTAT3     (*(volatile unsigned *)0x4b0000d4) //DMA 3 Status
    #define rDCSRC3     (*(volatile unsigned *)0x4b0000d8) //DMA 3 Current source
    #define rDCDST3     (*(volatile unsigned *)0x4b0000dc) //DMA 3 Current destination
    #define rDMASKTRIG3 (*(volatile unsigned *)0x4b0000e0) //DMA 3 Mask trigger
    // CLOCK & POWER MANAGEMENT
    #define rLOCKTIME   (*(volatile unsigned *)0x4c000000) //PLL lock time counter
    #define rMPLLCON    (*(volatile unsigned *)0x4c000004) //MPLL Control
    #define rUPLLCON    (*(volatile unsigned *)0x4c000008) //UPLL Control
    #define rCLKCON     (*(volatile unsigned *)0x4c00000c) //Clock generator control
    #define rCLKSLOW    (*(volatile unsigned *)0x4c000010) //Slow clock control
    #define rCLKDIVN    (*(volatile unsigned *)0x4c000014) //Clock divider control
    // LCD CONTROLLER
    #define rLCDCON1    (*(volatile unsigned *)0x4d000000) //LCD control 1
    #define rLCDCON2    (*(volatile unsigned *)0x4d000004) //LCD control 2
    #define rLCDCON3    (*(volatile unsigned *)0x4d000008) //LCD control 3
    #define rLCDCON4    (*(volatile unsigned *)0x4d00000c) //LCD control 4
    #define rLCDCON5    (*(volatile unsigned *)0x4d000010) //LCD control 5
    #define rLCDSADDR1  (*(volatile unsigned *)0x4d000014) //STN/TFT Frame buffer start address 1
    #define rLCDSADDR2  (*(volatile unsigned *)0x4d000018) //STN/TFT Frame buffer start address 2
    #define rLCDSADDR3  (*(volatile unsigned *)0x4d00001c) //STN/TFT Virtual screen address set
    #define rREDLUT     (*(volatile unsigned *)0x4d000020) //STN Red lookup table
    #define rGREENLUT   (*(volatile unsigned *)0x4d000024) //STN Green lookup table 
    #define rBLUELUT    (*(volatile unsigned *)0x4d000028) //STN Blue lookup table
    #define rDITHMODE   (*(volatile unsigned *)0x4d00004c) //STN Dithering mode
    #define rTPAL       (*(volatile unsigned *)0x4d000050) //TFT Temporary palette
    #define rLCDINTPND  (*(volatile unsigned *)0x4d000054) //LCD Interrupt pending
    #define rLCDSRCPND  (*(volatile unsigned *)0x4d000058) //LCD Interrupt source
    #define rLCDINTMSK  (*(volatile unsigned *)0x4d00005c) //LCD Interrupt mask
    #define rLPCSEL     (*(volatile unsigned *)0x4d000060) //LPC3600 Control
    #define PALETTE     0x4d000400                         //Palette start address
    // NAND flash
    #define rNFCONF     (*(volatile unsigned *)0x4e000000)      //NAND Flash configuration
    #define rNFCMD      (*(volatile U8 *)0x4e000004)            //NADD Flash command
    #define rNFADDR     (*(volatile U8 *)0x4e000008)            //NAND Flash address
    #define rNFDATA     (*(volatile U8 *)0x4e00000c)            //NAND Flash data
    #define rNFSTAT     (*(volatile unsigned *)0x4e000010)      //NAND Flash operation status
    #define rNFECC      (*(volatile unsigned *)0x4e000014)      //NAND Flash ECC
    #define rNFECC0     (*(volatile U8  *)0x4e000014)
    #define rNFECC1     (*(volatile U8  *)0x4e000015)
    #define rNFECC2     (*(volatile U8  *)0x4e000016)
    // UART
    #define rULCON0     (*(volatile unsigned *)0x50000000) //UART 0 Line control
    #define rUCON0      (*(volatile unsigned *)0x50000004) //UART 0 Control
    #define rUFCON0     (*(volatile unsigned *)0x50000008) //UART 0 FIFO control
    #define rUMCON0     (*(volatile unsigned *)0x5000000c) //UART 0 Modem control
    #define rUTRSTAT0   (*(volatile unsigned *)0x50000010) //UART 0 Tx/Rx status
    #define rUERSTAT0   (*(volatile unsigned *)0x50000014) //UART 0 Rx error status
    #define rUFSTAT0    (*(volatile unsigned *)0x50000018) //UART 0 FIFO status
    #define rUMSTAT0    (*(volatile unsigned *)0x5000001c) //UART 0 Modem status
    #define rUBRDIV0    (*(volatile unsigned *)0x50000028) //UART 0 Baud rate divisor
    #define rULCON1     (*(volatile unsigned *)0x50004000) //UART 1 Line control
    #define rUCON1      (*(volatile unsigned *)0x50004004) //UART 1 Control
    #define rUFCON1     (*(volatile unsigned *)0x50004008) //UART 1 FIFO control
    #define rUMCON1     (*(volatile unsigned *)0x5000400c) //UART 1 Modem control
    #define rUTRSTAT1   (*(volatile unsigned *)0x50004010) //UART 1 Tx/Rx status
    #define rUERSTAT1   (*(volatile unsigned *)0x50004014) //UART 1 Rx error status
    #define rUFSTAT1    (*(volatile unsigned *)0x50004018) //UART 1 FIFO status
    #define rUMSTAT1    (*(volatile unsigned *)0x5000401c) //UART 1 Modem status
    #define rUBRDIV1    (*(volatile unsigned *)0x50004028) //UART 1 Baud rate divisor
    #define rULCON2     (*(volatile unsigned *)0x50008000) //UART 2 Line control
    #define rUCON2      (*(volatile unsigned *)0x50008004) //UART 2 Control
    #define rUFCON2     (*(volatile unsigned *)0x50008008) //UART 2 FIFO control
    #define rUMCON2     (*(volatile unsigned *)0x5000800c) //UART 2 Modem control
    #define rUTRSTAT2   (*(volatile unsigned *)0x50008010) //UART 2 Tx/Rx status
    #define rUERSTAT2   (*(volatile unsigned *)0x50008014) //UART 2 Rx error status
    #define rUFSTAT2    (*(volatile unsigned *)0x50008018) //UART 2 FIFO status
    #define rUMSTAT2    (*(volatile unsigned *)0x5000801c) //UART 2 Modem status
    #define rUBRDIV2    (*(volatile unsigned *)0x50008028) //UART 2 Baud rate divisor
    #ifdef __BIG_ENDIAN
    #define rUTXH0      (*(volatile unsigned char *)0x50000023) //UART 0 Transmission Hold
    #define rURXH0      (*(volatile unsigned char *)0x50000027) //UART 0 Receive buffer
    #define rUTXH1      (*(volatile unsigned char *)0x50004023) //UART 1 Transmission Hold
    #define rURXH1      (*(volatile unsigned char *)0x50004027) //UART 1 Receive buffer
    #define rUTXH2      (*(volatile unsigned char *)0x50008023) //UART 2 Transmission Hold
    #define rURXH2      (*(volatile unsigned char *)0x50008027) //UART 2 Receive buffer
    #define WrUTXH0(ch) (*(volatile unsigned char *)0x50000023)=(unsigned char)(ch)
    #define RdURXH0()   (*(volatile unsigned char *)0x50000027)
    #define WrUTXH1(ch) (*(volatile unsigned char *)0x50004023)=(unsigned char)(ch)
    #define RdURXH1()   (*(volatile unsigned char *)0x50004027)
    #define WrUTXH2(ch) (*(volatile unsigned char *)0x50008023)=(unsigned char)(ch)
    #define RdURXH2()   (*(volatile unsigned char *)0x50008027)
    #define UTXH0       (0x50000020+3)  //Byte_access address by DMA
    #define URXH0       (0x50000024+3)
    #define UTXH1       (0x50004020+3)
    #define URXH1       (0x50004024+3)
    #define UTXH2       (0x50008020+3)
    #define URXH2       (0x50008024+3)
    #else //Little Endian
    #define rUTXH0 (*(volatile unsigned char *)0x50000020) //UART 0 Transmission Hold
    #define rURXH0 (*(volatile unsigned char *)0x50000024) //UART 0 Receive buffer
    #define rUTXH1 (*(volatile unsigned char *)0x50004020) //UART 1 Transmission Hold
    #define rURXH1 (*(volatile unsigned char *)0x50004024) //UART 1 Receive buffer
    #define rUTXH2 (*(volatile unsigned char *)0x50008020) //UART 2 Transmission Hold
    #define rURXH2 (*(volatile unsigned char *)0x50008024) //UART 2 Receive buffer
    #define WrUTXH0(ch) (*(volatile unsigned char *)0x50000020)=(unsigned char)(ch)
    #define RdURXH0()   (*(volatile unsigned char *)0x50000024)
    #define WrUTXH1(ch) (*(volatile unsigned char *)0x50004020)=(unsigned char)(ch)
    #define RdURXH1()   (*(volatile unsigned char *)0x50004024)
    #define WrUTXH2(ch) (*(volatile unsigned char *)0x50008020)=(unsigned char)(ch)
    #define RdURXH2()   (*(volatile unsigned char *)0x50008024)
    #define UTXH0       (0x50000020)    //Byte_access address by DMA
    #define URXH0       (0x50000024)
    #define UTXH1       (0x50004020)
    #define URXH1       (0x50004024)
    #define UTXH2       (0x50008020)
    #define URXH2       (0x50008024)
    #endif
    // PWM TIMER
    #define rTCFG0  (*(volatile unsigned *)0x51000000) //Timer 0 configuration
    #define rTCFG1  (*(volatile unsigned *)0x51000004) //Timer 1 configuration
    #define rTCON   (*(volatile unsigned *)0x51000008) //Timer control
    #define rTCNTB0 (*(volatile unsigned *)0x5100000c) //Timer count buffer 0
    #define rTCMPB0 (*(volatile unsigned *)0x51000010) //Timer compare buffer 0
    #define rTCNTO0 (*(volatile unsigned *)0x51000014) //Timer count observation 0
    #define rTCNTB1 (*(volatile unsigned *)0x51000018) //Timer count buffer 1
    #define rTCMPB1 (*(volatile unsigned *)0x5100001c) //Timer compare buffer 1
    #define rTCNTO1 (*(volatile unsigned *)0x51000020) //Timer count observation 1
    #define rTCNTB2 (*(volatile unsigned *)0x51000024) //Timer count buffer 2
    #define rTCMPB2 (*(volatile unsigned *)0x51000028) //Timer compare buffer 2
    #define rTCNTO2 (*(volatile unsigned *)0x5100002c) //Timer count observation 2
    #define rTCNTB3 (*(volatile unsigned *)0x51000030) //Timer count buffer 3
    #define rTCMPB3 (*(volatile unsigned *)0x51000034) //Timer compare buffer 3
    #define rTCNTO3 (*(volatile unsigned *)0x51000038) //Timer count observation 3
    #define rTCNTB4 (*(volatile unsigned *)0x5100003c) //Timer count buffer 4
    #define rTCNTO4 (*(volatile unsigned *)0x51000040) //Timer count observation 4
    // USB DEVICE
    #ifdef __BIG_ENDIAN
   // <ERROR IF BIG_ENDIAN>
    #define rFUNC_ADDR_REG     (*(volatile unsigned char *)0x52000143) //Function address
    #define rPWR_REG           (*(volatile unsigned char *)0x52000147) //Power management
    #define rEP_INT_REG        (*(volatile unsigned char *)0x5200014b) //EP Interrupt pending and clear
    #define rUSB_INT_REG       (*(volatile unsigned char *)0x5200015b) //USB Interrupt pending and clear
    #define rEP_INT_EN_REG     (*(volatile unsigned char *)0x5200015f) //Interrupt enable
    #define rUSB_INT_EN_REG    (*(volatile unsigned char *)0x5200016f)
    #define rFRAME_NUM1_REG    (*(volatile unsigned char *)0x52000173) //Frame number lower byte
    #define rFRAME_NUM2_REG    (*(volatile unsigned char *)0x52000177) //Frame number higher byte
    #define rINDEX_REG         (*(volatile unsigned char *)0x5200017b) //Register index
    #define rMAXP_REG          (*(volatile unsigned char *)0x52000183) //Endpoint max packet
    #define rEP0_CSR           (*(volatile unsigned char *)0x52000187) //Endpoint 0 status
    #define rIN_CSR1_REG       (*(volatile unsigned char *)0x52000187) //In endpoint control status
    #define rIN_CSR2_REG       (*(volatile unsigned char *)0x5200018b)
    #define rOUT_CSR1_REG      (*(volatile unsigned char *)0x52000193) //Out endpoint control status
    #define rOUT_CSR2_REG      (*(volatile unsigned char *)0x52000197)
    #define rOUT_FIFO_CNT1_REG (*(volatile unsigned char *)0x5200019b) //Endpoint out write count
    #define rOUT_FIFO_CNT2_REG (*(volatile unsigned char *)0x5200019f)
    #define rEP0_FIFO          (*(volatile unsigned char *)0x520001c3) //Endpoint 0 FIFO
    #define rEP1_FIFO          (*(volatile unsigned char *)0x520001c7) //Endpoint 1 FIFO
    #define rEP2_FIFO          (*(volatile unsigned char *)0x520001cb) //Endpoint 2 FIFO
    #define rEP3_FIFO          (*(volatile unsigned char *)0x520001cf) //Endpoint 3 FIFO
    #define rEP4_FIFO          (*(volatile unsigned char *)0x520001d3) //Endpoint 4 FIFO
    #define rEP1_DMA_CON       (*(volatile unsigned char *)0x52000203) //EP1 DMA interface control
    #define rEP1_DMA_UNIT      (*(volatile unsigned char *)0x52000207) //EP1 DMA Tx unit counter
    #define rEP1_DMA_FIFO      (*(volatile unsigned char *)0x5200020b) //EP1 DMA Tx FIFO counter
    #define rEP1_DMA_TTC_L     (*(volatile unsigned char *)0x5200020f) //EP1 DMA total Tx counter
    #define rEP1_DMA_TTC_M     (*(volatile unsigned char *)0x52000213)
    #define rEP1_DMA_TTC_H     (*(volatile unsigned char *)0x52000217)
    #define rEP2_DMA_CON       (*(volatile unsigned char *)0x5200021b) //EP2 DMA interface control
    #define rEP2_DMA_UNIT      (*(volatile unsigned char *)0x5200021f) //EP2 DMA Tx unit counter
    #define rEP2_DMA_FIFO      (*(volatile unsigned char *)0x52000223) //EP2 DMA Tx FIFO counter
    #define rEP2_DMA_TTC_L     (*(volatile unsigned char *)0x52000227) //EP2 DMA total Tx counter
    #define rEP2_DMA_TTC_M     (*(volatile unsigned char *)0x5200022b)
    #define rEP2_DMA_TTC_H     (*(volatile unsigned char *)0x5200022f)
    #define rEP3_DMA_CON       (*(volatile unsigned char *)0x52000243) //EP3 DMA interface control
    #define rEP3_DMA_UNIT      (*(volatile unsigned char *)0x52000247) //EP3 DMA Tx unit counter
    #define rEP3_DMA_FIFO      (*(volatile unsigned char *)0x5200024b) //EP3 DMA Tx FIFO counter
    #define rEP3_DMA_TTC_L     (*(volatile unsigned char *)0x5200024f) //EP3 DMA total Tx counter
    #define rEP3_DMA_TTC_M     (*(volatile unsigned char *)0x52000253)
    #define rEP3_DMA_TTC_H     (*(volatile unsigned char *)0x52000257)
    #define rEP4_DMA_CON       (*(volatile unsigned char *)0x5200025b) //EP4 DMA interface control
    #define rEP4_DMA_UNIT      (*(volatile unsigned char *)0x5200025f) //EP4 DMA Tx unit counter
    #define rEP4_DMA_FIFO      (*(volatile unsigned char *)0x52000263) //EP4 DMA Tx FIFO counter
    #define rEP4_DMA_TTC_L     (*(volatile unsigned char *)0x52000267) //EP4 DMA total Tx counter
    #define rEP4_DMA_TTC_M     (*(volatile unsigned char *)0x5200026b)
    #define rEP4_DMA_TTC_H     (*(volatile unsigned char *)0x5200026f)
    #else  // Little Endian
    #define rFUNC_ADDR_REG     (*(volatile unsigned char *)0x52000140) //Function address
    #define rPWR_REG           (*(volatile unsigned char *)0x52000144) //Power management
    #define rEP_INT_REG        (*(volatile unsigned char *)0x52000148) //EP Interrupt pending and clear
    #define rUSB_INT_REG       (*(volatile unsigned char *)0x52000158) //USB Interrupt pending and clear
    #define rEP_INT_EN_REG     (*(volatile unsigned char *)0x5200015c) //Interrupt enable
    #define rUSB_INT_EN_REG    (*(volatile unsigned char *)0x5200016c)
    #define rFRAME_NUM1_REG    (*(volatile unsigned char *)0x52000170) //Frame number lower byte
    #define rFRAME_NUM2_REG    (*(volatile unsigned char *)0x52000174) //Frame number higher byte
    #define rINDEX_REG         (*(volatile unsigned char *)0x52000178) //Register index
    #define rMAXP_REG          (*(volatile unsigned char *)0x52000180) //Endpoint max packet
    #define rEP0_CSR           (*(volatile unsigned char *)0x52000184) //Endpoint 0 status
    #define rIN_CSR1_REG       (*(volatile unsigned char *)0x52000184) //In endpoint control status
    #define rIN_CSR2_REG       (*(volatile unsigned char *)0x52000188)
    #define rOUT_CSR1_REG      (*(volatile unsigned char *)0x52000190) //Out endpoint control status
    #define rOUT_CSR2_REG      (*(volatile unsigned char *)0x52000194)
    #define rOUT_FIFO_CNT1_REG (*(volatile unsigned char *)0x52000198) //Endpoint out write count
    #define rOUT_FIFO_CNT2_REG (*(volatile unsigned char *)0x5200019c)
    #define rEP0_FIFO          (*(volatile unsigned char *)0x520001c0) //Endpoint 0 FIFO
    #define rEP1_FIFO          (*(volatile unsigned char *)0x520001c4) //Endpoint 1 FIFO
    #define rEP2_FIFO          (*(volatile unsigned char *)0x520001c8) //Endpoint 2 FIFO
    #define rEP3_FIFO          (*(volatile unsigned char *)0x520001cc) //Endpoint 3 FIFO
    #define rEP4_FIFO          (*(volatile unsigned char *)0x520001d0) //Endpoint 4 FIFO
    #define rEP1_DMA_CON       (*(volatile unsigned char *)0x52000200) //EP1 DMA interface control
    #define rEP1_DMA_UNIT      (*(volatile unsigned char *)0x52000204) //EP1 DMA Tx unit counter
    #define rEP1_DMA_FIFO      (*(volatile unsigned char *)0x52000208) //EP1 DMA Tx FIFO counter
    #define rEP1_DMA_TTC_L     (*(volatile unsigned char *)0x5200020c) //EP1 DMA total Tx counter
    #define rEP1_DMA_TTC_M     (*(volatile unsigned char *)0x52000210)
    #define rEP1_DMA_TTC_H     (*(volatile unsigned char *)0x52000214)
    #define rEP2_DMA_CON       (*(volatile unsigned char *)0x52000218) //EP2 DMA interface control
    #define rEP2_DMA_UNIT      (*(volatile unsigned char *)0x5200021c) //EP2 DMA Tx unit counter
    #define rEP2_DMA_FIFO      (*(volatile unsigned char *)0x52000220) //EP2 DMA Tx FIFO counter
    #define rEP2_DMA_TTC_L     (*(volatile unsigned char *)0x52000224) //EP2 DMA total Tx counter
    #define rEP2_DMA_TTC_M     (*(volatile unsigned char *)0x52000228)
    #define rEP2_DMA_TTC_H     (*(volatile unsigned char *)0x5200022c)
    #define rEP3_DMA_CON       (*(volatile unsigned char *)0x52000240) //EP3 DMA interface control
    #define rEP3_DMA_UNIT      (*(volatile unsigned char *)0x52000244) //EP3 DMA Tx unit counter
    #define rEP3_DMA_FIFO      (*(volatile unsigned char *)0x52000248) //EP3 DMA Tx FIFO counter
    #define rEP3_DMA_TTC_L     (*(volatile unsigned char *)0x5200024c) //EP3 DMA total Tx counter
    #define rEP3_DMA_TTC_M     (*(volatile unsigned char *)0x52000250)
    #define rEP3_DMA_TTC_H     (*(volatile unsigned char *)0x52000254)
    #define rEP4_DMA_CON       (*(volatile unsigned char *)0x52000258) //EP4 DMA interface control
    #define rEP4_DMA_UNIT      (*(volatile unsigned char *)0x5200025c) //EP4 DMA Tx unit counter
    #define rEP4_DMA_FIFO      (*(volatile unsigned char *)0x52000260) //EP4 DMA Tx FIFO counter
    #define rEP4_DMA_TTC_L     (*(volatile unsigned char *)0x52000264) //EP4 DMA total Tx counter
    #define rEP4_DMA_TTC_M     (*(volatile unsigned char *)0x52000268)
    #define rEP4_DMA_TTC_H     (*(volatile unsigned char *)0x5200026c)
    #endif   // __BIG_ENDIAN
    // WATCH DOG TIMER
    #define rWTCON   (*(volatile unsigned *)0x53000000) //Watch-dog timer mode
    #define rWTDAT   (*(volatile unsigned *)0x53000004) //Watch-dog timer data
    #define rWTCNT   (*(volatile unsigned *)0x53000008) //Eatch-dog timer count
    // IIC
    #define rIICCON  (*(volatile unsigned *)0x54000000) //IIC control
    #define rIICSTAT (*(volatile unsigned *)0x54000004) //IIC status
    #define rIICADD  (*(volatile unsigned *)0x54000008) //IIC address
    #define rIICDS   (*(volatile unsigned *)0x5400000c) //IIC data shift
    // IIS
    #define rIISCON  (*(volatile unsigned *)0x55000000) //IIS Control
    #define rIISMOD  (*(volatile unsigned *)0x55000004) //IIS Mode
    #define rIISPSR  (*(volatile unsigned *)0x55000008) //IIS Prescaler
    #define rIISFCON (*(volatile unsigned *)0x5500000c) //IIS FIFO control
    #ifdef __BIG_ENDIAN
    #define IISFIFO  ((volatile unsigned short *)0x55000012) //IIS FIFO entry
    #else //Little Endian
    #define IISFIFO  ((volatile unsigned short *)0x55000010) //IIS FIFO entry
    #endif
    // I/O PORT 
    #define rGPACON    (*(volatile unsigned *)0x56000000) //Port A control
    #define rGPADAT    (*(volatile unsigned *)0x56000004) //Port A data
    #define rGPBCON    (*(volatile unsigned *)0x56000010) //Port B control
    #define rGPBDAT    (*(volatile unsigned *)0x56000014) //Port B data
    #define rGPBUP     (*(volatile unsigned *)0x56000018) //Pull-up control B
    #define rGPCCON    (*(volatile unsigned *)0x56000020) //Port C control
    #define rGPCDAT    (*(volatile unsigned *)0x56000024) //Port C data
    #define rGPCUP     (*(volatile unsigned *)0x56000028) //Pull-up control C
    #define rGPDCON    (*(volatile unsigned *)0x56000030) //Port D control
    #define rGPDDAT    (*(volatile unsigned *)0x56000034) //Port D data
    #define rGPDUP     (*(volatile unsigned *)0x56000038) //Pull-up control D
    #define rGPECON    (*(volatile unsigned *)0x56000040) //Port E control
    #define rGPEDAT    (*(volatile unsigned *)0x56000044) //Port E data
    #define rGPEUP     (*(volatile unsigned *)0x56000048) //Pull-up control E
    #define rGPFCON    (*(volatile unsigned *)0x56000050) //Port F control
    #define rGPFDAT    (*(volatile unsigned *)0x56000054) //Port F data
    #define rGPFUP     (*(volatile unsigned *)0x56000058) //Pull-up control F
    #define rGPGCON    (*(volatile unsigned *)0x56000060) //Port G control
    #define rGPGDAT    (*(volatile unsigned *)0x56000064) //Port G data
    #define rGPGUP     (*(volatile unsigned *)0x56000068) //Pull-up control G
    #define rGPHCON    (*(volatile unsigned *)0x56000070) //Port H control
    #define rGPHDAT    (*(volatile unsigned *)0x56000074) //Port H data
    #define rGPHUP     (*(volatile unsigned *)0x56000078) //Pull-up control H
    #define rMISCCR    (*(volatile unsigned *)0x56000080) //Miscellaneous control
    #define rDCLKCON   (*(volatile unsigned *)0x56000084) //DCLK0/1 control
    #define rEXTINT0   (*(volatile unsigned *)0x56000088) //External interrupt control register 0
    #define rEXTINT1   (*(volatile unsigned *)0x5600008c) //External interrupt control register 1
    #define rEXTINT2   (*(volatile unsigned *)0x56000090) //External interrupt control register 2
    #define rEINTFLT0  (*(volatile unsigned *)0x56000094) //Reserved
    #define rEINTFLT1  (*(volatile unsigned *)0x56000098) //Reserved
    #define rEINTFLT2  (*(volatile unsigned *)0x5600009c) //External interrupt filter control register 2
    #define rEINTFLT3  (*(volatile unsigned *)0x560000a0) //External interrupt filter control register 3
    #define rEINTMASK  (*(volatile unsigned *)0x560000a4) //External interrupt mask
    #define rEINTPEND  (*(volatile unsigned *)0x560000a8) //External interrupt pending
    #define rGSTATUS0  (*(volatile unsigned *)0x560000ac) //External pin status
    #define rGSTATUS1  (*(volatile unsigned *)0x560000b0) //Chip ID(0x32410000)
    #define rGSTATUS2  (*(volatile unsigned *)0x560000b4) //Reset type
    #define rGSTATUS3  (*(volatile unsigned *)0x560000b8) //Saved data0(32-bit) before entering POWER_OFF mode 
    #define rGSTATUS4  (*(volatile unsigned *)0x560000bc) //Saved data0(32-bit) before entering POWER_OFF mode 
    // RTC
    #ifdef __BIG_ENDIAN
    #define rRTCCON    (*(volatile unsigned char *)0x57000043) //RTC control
    #define rTICNT     (*(volatile unsigned char *)0x57000047) //Tick time count
    #define rRTCALM    (*(volatile unsigned char *)0x57000053) //RTC alarm control
    #define rALMSEC    (*(volatile unsigned char *)0x57000057) //Alarm second
    #define rALMMIN    (*(volatile unsigned char *)0x5700005b) //Alarm minute
    #define rALMHOUR   (*(volatile unsigned char *)0x5700005f) //Alarm Hour
    #define rALMDATE   (*(volatile unsigned char *)0x57000063) //Alarm day     <-- May 06, 2002 SOP
    #define rALMMON    (*(volatile unsigned char *)0x57000067) //Alarm month
    #define rALMYEAR   (*(volatile unsigned char *)0x5700006b) //Alarm year
    #define rRTCRST    (*(volatile unsigned char *)0x5700006f) //RTC round reset
    #define rBCDSEC    (*(volatile unsigned char *)0x57000073) //BCD second
    #define rBCDMIN    (*(volatile unsigned char *)0x57000077) //BCD minute
    #define rBCDHOUR   (*(volatile unsigned char *)0x5700007b) //BCD hour
    #define rBCDDATE   (*(volatile unsigned char *)0x5700007f) //BCD day       <-- May 06, 2002 SOP
    #define rBCDDAY    (*(volatile unsigned char *)0x57000083) //BCD date      <-- May 06, 2002 SOP
    #define rBCDMON    (*(volatile unsigned char *)0x57000087) //BCD month
    #define rBCDYEAR   (*(volatile unsigned char *)0x5700008b) //BCD year
    #else //Little Endian
    #define rRTCCON    (*(volatile unsigned char *)0x57000040) //RTC control
    #define rTICNT     (*(volatile unsigned char *)0x57000044) //Tick time count
    #define rRTCALM    (*(volatile unsigned char *)0x57000050) //RTC alarm control
    #define rALMSEC    (*(volatile unsigned char *)0x57000054) //Alarm second
    #define rALMMIN    (*(volatile unsigned char *)0x57000058) //Alarm minute
    #define rALMHOUR   (*(volatile unsigned char *)0x5700005c) //Alarm Hour
    #define rALMDATE   (*(volatile unsigned char *)0x57000060) //Alarm day      <-- May 06, 2002 SOP
    #define rALMMON    (*(volatile unsigned char *)0x57000064) //Alarm month
    #define rALMYEAR   (*(volatile unsigned char *)0x57000068) //Alarm year
    #define rRTCRST    (*(volatile unsigned char *)0x5700006c) //RTC round reset
    #define rBCDSEC    (*(volatile unsigned char *)0x57000070) //BCD second
    #define rBCDMIN    (*(volatile unsigned char *)0x57000074) //BCD minute
    #define rBCDHOUR   (*(volatile unsigned char *)0x57000078) //BCD hour
    #define rBCDDATE   (*(volatile unsigned char *)0x5700007c) //BCD day        <-- May 06, 2002 SOP
    #define rBCDDAY    (*(volatile unsigned char *)0x57000080) //BCD date       <-- May 06, 2002 SOP
    #define rBCDMON    (*(volatile unsigned char *)0x57000084) //BCD month
    #define rBCDYEAR   (*(volatile unsigned char *)0x57000088) //BCD year
    #endif  //RTC
    // ADC
    #define rADCCON    (*(volatile unsigned *)0x58000000) //ADC control
    #define rADCTSC    (*(volatile unsigned *)0x58000004) //ADC touch screen control
    #define rADCDLY    (*(volatile unsigned *)0x58000008) //ADC start or Interval Delay
    #define rADCDAT0   (*(volatile unsigned *)0x5800000c) //ADC conversion data 0
    #define rADCDAT1   (*(volatile unsigned *)0x58000010) //ADC conversion data 1                   
    // SPI          
    #define rSPCON0    (*(volatile unsigned *)0x59000000) //SPI0 control
    #define rSPSTA0    (*(volatile unsigned *)0x59000004) //SPI0 status
    #define rSPPIN0    (*(volatile unsigned *)0x59000008) //SPI0 pin control
    #define rSPPRE0    (*(volatile unsigned *)0x5900000c) //SPI0 baud rate prescaler
    #define rSPTDAT0   (*(volatile unsigned *)0x59000010) //SPI0 Tx data
    #define rSPRDAT0   (*(volatile unsigned *)0x59000014) //SPI0 Rx data
    #define rSPCON1    (*(volatile unsigned *)0x59000020) //SPI1 control
    #define rSPSTA1    (*(volatile unsigned *)0x59000024) //SPI1 status
    #define rSPPIN1    (*(volatile unsigned *)0x59000028) //SPI1 pin control
    #define rSPPRE1    (*(volatile unsigned *)0x5900002c) //SPI1 baud rate prescaler
    #define rSPTDAT1   (*(volatile unsigned *)0x59000030) //SPI1 Tx data
    #define rSPRDAT1   (*(volatile unsigned *)0x59000034) //SPI1 Rx data
    // SD Interface
    #define rSDICON     (*(volatile unsigned *)0x5a000000) //SDI control
    #define rSDIPRE     (*(volatile unsigned *)0x5a000004) //SDI baud rate prescaler
    #define rSDICARG    (*(volatile unsigned *)0x5a000008) //SDI command argument
    #define rSDICCON    (*(volatile unsigned *)0x5a00000c) //SDI command control
    #define rSDICSTA    (*(volatile unsigned *)0x5a000010) //SDI command status
    #define rSDIRSP0    (*(volatile unsigned *)0x5a000014) //SDI response 0
    #define rSDIRSP1    (*(volatile unsigned *)0x5a000018) //SDI response 1
    #define rSDIRSP2    (*(volatile unsigned *)0x5a00001c) //SDI response 2
    #define rSDIRSP3    (*(volatile unsigned *)0x5a000020) //SDI response 3
    #define rSDIDTIMER  (*(volatile unsigned *)0x5a000024) //SDI data/busy timer
    #define rSDIBSIZE   (*(volatile unsigned *)0x5a000028) //SDI block size
    #define rSDIDCON    (*(volatile unsigned *)0x5a00002c) //SDI data control
    #define rSDIDCNT    (*(volatile unsigned *)0x5a000030) //SDI data remain counter
    #define rSDIDSTA    (*(volatile unsigned *)0x5a000034) //SDI data status
    #define rSDIFSTA    (*(volatile unsigned *)0x5a000038) //SDI FIFO status
    #define rSDIIMSK    (*(volatile unsigned *)0x5a000040) //SDI interrupt mask
	
    #ifdef __BIG_ENDIAN
    #define rSDIDAT    (*(volatile unsigned *)0x5a00003f) //SDI data
    #define SDIDAT     0x5a00003f
    #else  // Little Endian
    #define rSDIDAT    (*(volatile unsigned *)0x5a00003c) //SDI data
    #define SDIDAT     0x5a00003c
    #endif   //SD Interface
	

    // ISR
    #define pISR_RESET     (*(unsigned *)(_ISR_STARTADDRESS+0x0))
    #define pISR_UNDEF     (*(unsigned *)(_ISR_STARTADDRESS+0x4))
    #define pISR_SWI       (*(unsigned *)(_ISR_STARTADDRESS+0x8))
    #define pISR_PABORT    (*(unsigned *)(_ISR_STARTADDRESS+0xc))
    #define pISR_DABORT    (*(unsigned *)(_ISR_STARTADDRESS+0x10))
    #define pISR_RESERVED  (*(unsigned *)(_ISR_STARTADDRESS+0x14))
    #define pISR_IRQ       (*(unsigned *)(_ISR_STARTADDRESS+0x18))
    #define pISR_FIQ       (*(unsigned *)(_ISR_STARTADDRESS+0x1c))
    #define pISR_EINT0     (*(unsigned *)(_ISR_STARTADDRESS+0x20))
    #define pISR_EINT1     (*(unsigned *)(_ISR_STARTADDRESS+0x24))
    #define pISR_EINT2     (*(unsigned *)(_ISR_STARTADDRESS+0x28))
    #define pISR_EINT3     (*(unsigned *)(_ISR_STARTADDRESS+0x2c))
    #define pISR_EINT4_7   (*(unsigned *)(_ISR_STARTADDRESS+0x30))
    #define pISR_EINT8_23  (*(unsigned *)(_ISR_STARTADDRESS+0x34))
    #define pISR_NOTUSED6  (*(unsigned *)(_ISR_STARTADDRESS+0x38))
    #define pISR_BAT_FLT   (*(unsigned *)(_ISR_STARTADDRESS+0x3c))
    #define pISR_TICK      (*(unsigned *)(_ISR_STARTADDRESS+0x40))
    #define pISR_WDT       (*(unsigned *)(_ISR_STARTADDRESS+0x44))
    #define pISR_TIMER0    (*(unsigned *)(_ISR_STARTADDRESS+0x48))
    #define pISR_TIMER1    (*(unsigned *)(_ISR_STARTADDRESS+0x4c))
    #define pISR_TIMER2    (*(unsigned *)(_ISR_STARTADDRESS+0x50))
    #define pISR_TIMER3    (*(unsigned *)(_ISR_STARTADDRESS+0x54))
    #define pISR_TIMER4    (*(unsigned *)(_ISR_STARTADDRESS+0x58))
    #define pISR_UART2     (*(unsigned *)(_ISR_STARTADDRESS+0x5c))
    #define pISR_LCD       (*(unsigned *)(_ISR_STARTADDRESS+0x60))
    #define pISR_DMA0      (*(unsigned *)(_ISR_STARTADDRESS+0x64))
    #define pISR_DMA1      (*(unsigned *)(_ISR_STARTADDRESS+0x68))
    #define pISR_DMA2      (*(unsigned *)(_ISR_STARTADDRESS+0x6c))
    #define pISR_DMA3      (*(unsigned *)(_ISR_STARTADDRESS+0x70))
    #define pISR_SDI       (*(unsigned *)(_ISR_STARTADDRESS+0x74))
    #define pISR_SPI0      (*(unsigned *)(_ISR_STARTADDRESS+0x78))
    #define pISR_UART1     (*(unsigned *)(_ISR_STARTADDRESS+0x7c))
    #define pISR_NOTUSED24 (*(unsigned *)(_ISR_STARTADDRESS+0x80))
    #define pISR_USBD      (*(unsigned *)(_ISR_STARTADDRESS+0x84))
    #define pISR_USBH      (*(unsigned *)(_ISR_STARTADDRESS+0x88))
    #define pISR_IIC       (*(unsigned *)(_ISR_STARTADDRESS+0x8c))
    #define pISR_UART0     (*(unsigned *)(_ISR_STARTADDRESS+0x90))
    #define pISR_SPI1      (*(unsigned *)(_ISR_STARTADDRESS+0x94))
    #define pISR_RTC       (*(unsigned *)(_ISR_STARTADDRESS+0x98))
    #define pISR_ADC       (*(unsigned *)(_ISR_STARTADDRESS+0x9c))
	*/
    //laputa 030731 begine-->
    //CLOCK SLOW CONTROL REGISTER ITEM 
    //it's turn off the M/UPLL for power management 
    #define UCLK_ON  (1<<7)    // 0:UCLK ON : ISERTED AUTOMATICALLY, 1: UCLK OFF
    #define MPLL_OFF (1<<5)    // 0:PLL TURN ON 1:PLL IS TURN OFF
    #define SLOW_BIT (1<<4)    // 0:FCLK=MPLL 1:SLOW MODE
    //laputa append end <--
    /* bits */
    #define CLKCON_SPI		(1<<18)
    #define CLKCON_IIS		(1<<17)
    #define CLKCON_IIC		(1<<16)
    #define CLKCON_ADC		(1<<15)
    #define CLKCON_RTC		(1<<14)
    #define CLKCON_GPIO		(1<<13)
    #define CLKCON_UART2		(1<<12)
    #define CLKCON_UART1		(1<<11)
    #define CLKCON_UART0		(1<<10)
    #define CLKCON_SDI		(1<<9)
    #define CLKCON_PWM		(1<<8)
    #define CLKCON_USBD		(1<<7)
    #define CLKCON_USBH		(1<<6)
    #define CLKCON_LCDC		(1<<5)
    #define CLKCON_NAND		(1<<4)
    #define CLKCON_POWEROFF		(1<<3)
    #define CLKCON_IDLE		(1<<2)
    #define MISCCR_USBPAD		(1 << 3)	/* use pads related USB for  0: USB slave, 1: USB host */
    #define MISCCR_USB0_SUSPEND	(1 << 12)	/* set USB port 0 to Sleep */
    #define MISCCR_USB1_SUSPEND	(1 << 13)	/* set USB port 1 to Sleep */
/*
// PENDING BIT
    #define BIT_EINT0      (0x1)
    #define BIT_EINT1      (0x1<<1)
    #define BIT_EINT2      (0x1<<2)
    #define BIT_EINT3      (0x1<<3)
    #define BIT_EINT4_7    (0x1<<4)
    #define BIT_EINT8_23   (0x1<<5)
    #define BIT_NOTUSED6   (0x1<<6)
    #define BIT_BAT_FLT    (0x1<<7)
    #define BIT_TICK       (0x1<<8)
    #define BIT_WDT        (0x1<<9)
    #define BIT_TIMER0     (0x1<<10)
    #define BIT_TIMER1     (0x1<<11)
    #define BIT_TIMER2     (0x1<<12)
    #define BIT_TIMER3     (0x1<<13)
    #define BIT_TIMER4     (0x1<<14)
    #define BIT_UART2      (0x1<<15)
    #define BIT_LCD        (0x1<<16)
    #define BIT_DMA0       (0x1<<17)
    #define BIT_DMA1       (0x1<<18)
    #define BIT_DMA2       (0x1<<19)
    #define BIT_DMA3       (0x1<<20)
    #define BIT_SDI        (0x1<<21)
    #define BIT_SPI0       (0x1<<22)
    #define BIT_UART1      (0x1<<23)
    #define BIT_NOTUSED24  (0x1<<24)
    #define BIT_USBD       (0x1<<25)
    #define BIT_USBH       (0x1<<26)
    #define BIT_IIC        (0x1<<27)
    #define BIT_UART0      (0x1<<28)
    #define BIT_SPI1       (0x1<<29)
    #define BIT_RTC        (0x1<<30)
    #define BIT_ADC        (0x1<<31)
    #define BIT_ALLMSK     (0xffffffff)
    #define BIT_SUB_ALLMSK (0x7ff)
    #define BIT_SUB_ADC    (0x1<<10)
    #define BIT_SUB_TC     (0x1<<9)
    #define BIT_SUB_ERR2   (0x1<<8)
    #define BIT_SUB_TXD2   (0x1<<7)
    #define BIT_SUB_RXD2   (0x1<<6)
    #define BIT_SUB_ERR1   (0x1<<5)
    #define BIT_SUB_TXD1   (0x1<<4)
    #define BIT_SUB_RXD1   (0x1<<3)
    #define BIT_SUB_ERR0   (0x1<<2)
    #define BIT_SUB_TXD0   (0x1<<1)
    #define BIT_SUB_RXD0   (0x1<<0)
	
    #define ClearPending(bit) \
    {\
        \
        rSRCPND = bit;\
        rINTPND = bit;\
        rINTPND;\
    }       
    //Wait until rINTPND is changed for the case that the ISR is very short.
*/

#ifdef __cplusplus
}
#endif

#endif  //__2410ADDR_H___
