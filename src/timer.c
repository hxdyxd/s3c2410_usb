#include "def.h"
#include "2410addr.h"
#include "2410slib.h"
#include "2410lib.h"
#include "2410slib.h"
#include "timer.h"

#define	EXT_XTAL_FREQ	12000000
#define	BIOS_TIMER_FREQ	100

U32 SYS_FCLK, SYS_HCLK, SYS_PCLK;

static U8 sCLKDIVN, SlowMode;
static U8 chg_os_timer, os_timer_run, chg_bios_timer, bios_timer_run;
static U16 os_timer_rld, bios_timer_rld;
static U16 Timer4Cnt, Timer4Freq;
static U16 DoBiosEvent;

static void SetHclkPclk()
{
	if(sCLKDIVN&2)
		SYS_HCLK = SYS_FCLK>>1;
	else
		SYS_HCLK = SYS_FCLK;
	
	if(sCLKDIVN&1)
		SYS_PCLK = SYS_HCLK>>1;
	else
		SYS_PCLK = SYS_HCLK;
}

U8 SetSysFclk(U32 val)
{
	U32 i, freq;
	U8 mdiv, pdiv, sdiv;
	
	if(SlowMode)
		return FALSE;
	
	mdiv = (val>>12)&0xff;
	pdiv = (val>>4)&0x3f;
	sdiv = val&0x3;
	
	i = (pdiv+2);
	while(sdiv--)
		i *= 2;
		
	freq = ((mdiv+8)*EXT_XTAL_FREQ)/i;
	if(freq>=(3*EXT_XTAL_FREQ))	{
		rMPLLCON = val;
		SYS_FCLK = freq;
		SetHclkPclk();
		if(os_timer_run) {
			os_timer_rld = SYS_PCLK/(8*4*Timer4Freq)-1;
			chg_os_timer = 1;
		}
		if(bios_timer_run) {
			bios_timer_rld = SYS_PCLK/(8*4*BIOS_TIMER_FREQ)-1;
			chg_bios_timer = 1;
		}
		return TRUE;
	}
	return FALSE;
}

void SetClockDivider(int hdivn, int pdivn)
{
     // hdivn,pdivn FCLK:HCLK:PCLK
     //     0,0         1:1:1 
     //     0,1         1:1:2 
     //     1,0         1:2:2
     //     1,1         1:2:4
    if(SlowMode)
    	return;
     
	hdivn &= 1;
	pdivn &= 1;
	sCLKDIVN = (hdivn<<1)|pdivn;
	rCLKDIVN = sCLKDIVN;

    if(hdivn)
        MMU_SetAsyncBusMode();
    else 
        MMU_SetFastBusMode();
        
	SetHclkPclk();
}

void ChangeSlowMode(U16 mode)
{
	static U32 save_clk, save_div;
	U8 div;
	
	if(mode&CLKSLOW_SLOW_BIT) {		//enter slow mode
		div = mode&7;
		
		save_clk = SYS_FCLK;
		save_div = sCLKDIVN;
		if(div)
			SYS_FCLK = EXT_XTAL_FREQ/(2*div);
		else
			SYS_FCLK = EXT_XTAL_FREQ;
		SetClockDivider(0, 0);
		rCLKSLOW = mode;
		SlowMode = 1;
	} else {						//exit slow mode
		SlowMode = 0;				//clear SlowMode before SetClockDivider
		mode &= ~CLKSLOW_MPLL_OFF;	//must enable MPLL
		SYS_FCLK = save_clk;
		SetClockDivider(save_div>>1, save_div);
		rCLKSLOW = mode;
	}
}

/**************************************************************/
static void __irq ISR_Timer4(void)
{	
//	ClearPending(BIT_TIMER4);
	register i;
	
	rSRCPND = BIT_TIMER4;
	rINTPND = BIT_TIMER4;
	i = rINTPND;
/*
	Timer4Cnt--;
	if(!Timer4Cnt) {
		Timer4Cnt = Timer4Freq;
		led ^= 0xf;
		Led_Display(led);		
	}
*/	
	if(chg_os_timer) {
		chg_os_timer = 0;
		
		rTCNTB4 = os_timer_rld;
		rTCON  &= 0xff0fffff;	//stop Timer4
		rTCON  |= 0x00700000;	//auto-reload, update TCNTB4, start Timer4
		rTCON  &= 0xffdfffff;
	}
}

U32 OpenOsTimer(U16 OSTimer)
{
	Timer4Freq = 1000/OSTimer;
	Timer4Cnt  = Timer4Freq;

	rTCFG0 &= 0xffff00ff;
	rTCFG0 |= 7<<8;		//8 prescaler
	rTCFG1 &= 0xfff0ffff;
	rTCFG1 |= 1<<16;	//mux = 1/4
	rTCNTB4 = SYS_PCLK/(8*4*Timer4Freq)-1;
	rTCON  &= 0xff0fffff;	//stop Timer4
	rTCON  |= 0x00700000;	//auto-reload, update TCNTB4, start Timer4
	rTCON  &= 0xffdfffff;		
		
	os_timer_run = 1;	
	pISR_TIMER4  = (U32)ISR_Timer4;
	ClearPending(BIT_TIMER4);	
	EnableIrq(BIT_TIMER4);
	
	return 14;
}

__inline void ClearOsTimerPnd(void)
{
//	ClearPending(BIT_TIMER4);
	register i;
	
	rSRCPND = BIT_TIMER4;
	rINTPND = BIT_TIMER4;
	i = rINTPND;
}

/*********************************************************/
static struct{
	U16 cnt;
	U16 rld;
	void (*proc)(U32);
}BiosTimerEvent[MaxBiosTimerEvent];

static U16 bios_tick_run = 0;
static U32 bios_tick_cnt;

static __irq void ISR_Timer3(void)
{
	U32 i;
	register r;

//	ClearPending(BIT_TIMER3);
	rSRCPND = BIT_TIMER3;
	rINTPND = BIT_TIMER3;
	r = rINTPND;

	if(chg_bios_timer) {
		chg_bios_timer = 0;

		rTCNTB3 = bios_timer_rld;
		rTCON  &= 0xfff0ffff;
		rTCON  |= 0x000b0000;
		rTCON  &= 0xfffdffff;
	}
	
	bios_tick_cnt++;	//whenever bios_tick_run is 1 or 0, bios_tick_cnt++

	for(i=0; i<MaxBiosTimerEvent; i++) {
		if(BiosTimerEvent[i].proc) {
			BiosTimerEvent[i].cnt--;
			if(!BiosTimerEvent[i].cnt) {		
				BiosTimerEvent[i].cnt = BiosTimerEvent[i].rld;				
				(*BiosTimerEvent[i].proc)(i);
			}
		}
	}

	if(!(DoBiosEvent||bios_tick_run)) {
		bios_timer_run = 0;
		DisableIrq(BIT_TIMER3);
	}
}

static void OpenBiosTimer(void)
{
	U16 i;
	
	if(!bios_timer_run) {

		DoBiosEvent = 0;
		for(i=0; i<MaxBiosTimerEvent; i++) {
			BiosTimerEvent[i].proc = 0;
		}
	
		if(!os_timer_run) {
			rTCFG0 &= 0xffff00ff;
			rTCFG0 |= 7<<8;		//8 prescaler
		}
		
		rTCFG1 &= 0xffff0fff;
		rTCFG1 |= 1<<12;	//mux = 1/4
		rTCNTB3 = SYS_PCLK/(8*4*BIOS_TIMER_FREQ)-1;
		rTCON  &= 0xfff0ffff;
		rTCON  |= 0x000b0000;
		rTCON  &= 0xfffdffff;
	
		bios_timer_run = 1;
		pISR_TIMER3    = (U32)ISR_Timer3;
		ClearPending(BIT_TIMER3);
		EnableIrq(BIT_TIMER3);
	}
}

int RequestBiosTimerEvent(U16 rld, void (*proc)(U32))
{
	int i=-1;	
	U32 r;

	EnterCritical(&r);
	
	if(!bios_timer_run)
		OpenBiosTimer();
				
	for(i=0; i<MaxBiosTimerEvent; i++)	
		if(!BiosTimerEvent[i].proc) {						
			BiosTimerEvent[i].cnt  = rld;
			BiosTimerEvent[i].rld  = rld;
			BiosTimerEvent[i].proc = proc;
			DoBiosEvent++;
			break;
		}
		
	ExitCritical(&r);
			
	return i;
}

void ReleaseBiosTimerEvent(U16 number)
{
	U32 r;
	
	EnterCritical(&r);
	
	if((number<MaxBiosTimerEvent)&&(BiosTimerEvent[number].proc)) {
		BiosTimerEvent[number].proc = 0;
		if(DoBiosEvent)
			DoBiosEvent--;
	}
	
	ExitCritical(&r);
}

void RtcOpenTick(void)
{
	if(!bios_timer_run)
		OpenBiosTimer();

	if(!bios_tick_run)
		bios_tick_cnt = 0;

	bios_tick_run = 1;
}

void RtcCloseTick(void)
{
	bios_tick_run = 0;
}

BYTE RtcReadTick(U32 *pTicks)
{
	BYTE r;

	if(!bios_tick_run) {	
		*pTicks = 0;
		return 0;
	}

	r = (bios_tick_cnt!=*pTicks);
	*pTicks = bios_tick_cnt;

	return r;
}