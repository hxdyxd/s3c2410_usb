#ifndef _TIMER_H
#define	_TIMER_H

#define	FCLK_260M		((252<<12)|(4<<4)|1)
#define	FCLK_240M		((232<<12)|(4<<4)|1)
#define	FCLK_220M		((212<<12)|(4<<4)|1)
#define	FCLK_200M		((192<<12)|(4<<4)|1)
#define	FCLK_180M		((172<<12)|(4<<4)|1)
#define	FCLK_160M		((152<<12)|(4<<4)|1)
#define	FCLK_140M		((132<<12)|(4<<4)|1)
#define	FCLK_120M		((112<<12)|(4<<4)|1)
#define	FCLK_100M		((92<<12)|(4<<4)|1)
#define	FCLK_80M		((72<<12)|(4<<4)|1)
#define	FCLK_60M		((52<<12)|(4<<4)|1)
#define	FCLK_48M		((40<<12)|(4<<4)|1)
#define	FCLK_36M		((28<<12)|(4<<4)|1)

#define	DFT_FCLK_VAL	FCLK_200M

#define	CLKSLOW_SLOW_BIT	0x10
#define	CLKSLOW_UPLL_OFF	0x80
#define	CLKSLOW_MPLL_OFF	0x20
#define	CLKSLOW_PLL_OFF		0xb0
#define	CLKSLOW_PLL_ON		0
#define	EnableMPLL			(rCLKSLOW &= ~CLKSLOW_MPLL_OFF)
#define	DisableMPLL			(rCLKSLOW |= CLKSLOW_MPLL_OFF)
#define	EnableUPLL()		(rCLKSLOW &= ~CLKSLOW_UPLL_OFF)
#define	DisableUPLL()		(rCLKSLOW |= CLKSLOW_UPLL_OFF)

#define	CLOCK_SPI	(1<<18)
#define	CLOCK_IIS	(1<<17)
#define	CLOCK_IIC	(1<<16)
#define	CLOCK_ADC	(1<<15)
#define	CLOCK_RTC	(1<<14)
#define	CLOCK_GPIO	(1<<13)
#define	CLOCK_UART2	(1<<12)
#define	CLOCK_UART1	(1<<11)
#define	CLOCK_UART0	(1<<10)
#define	CLOCK_SDI	(1<<9)
#define	CLOCK_TIMER	(1<<8)
#define	CLOCK_USBD	(1<<7)
#define	CLOCK_USBH	(1<<6)
#define	CLOCK_LCD	(1<<5)
#define	CLOCK_NAND	(1<<4)
//#define	PWROFF		(1<<3)
//#define	IDLE_MODE	(1<<2)
#define	CLOCK_ALL	0xffff0
#define	EnableModuleClock(m)	(rCLKCON |= (m))
#define	DisableModuleClock(m)	(rCLKCON &= ~(m))



#define	MaxBiosTimerEvent	8

U8 SetSysFclk(U32 val);
void SetClockDivider(int hdivn, int pdivn);
U32 OpenOsTimer(U16 OSTimer);
void ReleaseOsTimer(void);
__inline void ClearOsTimerPnd(void);
//void OpenBiosTimer(void);
int RequestBiosTimerEvent(U16 rld, void (*proc)(U32));
void ReleaseBiosTimerEvent(U16 number);

void RtcOpenTick(void);
void RtcCloseTick(void);
BYTE RtcReadTick(U32 *pTicks);

void ChangeSlowMode(U16 mode);

extern U32 SYS_FCLK, SYS_HCLK, SYS_PCLK;

#endif	/* _TIMER_H */