#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "rtcapi.h"
#include "alarm.h"

/*
ALARM初始化，当系统地一次运行或者复位时运行。用于重组ALARM数据并启动ALARM。 
输入：无 
输出：TRUE--成功；FALSE--失败
*/
char ALM_Init(void)
{
	return 0;
}

/*
APP注册，如果APP要使用ALARM功能，则必须先应用这个函数注册。 
输入：pAlmApp--要增加的APP注册项结构指针 
输出：TRUE--成功；FALSE--失败 
*/
char ALM_RegisterApp(ALM_APP_T almApp)
{
	return 0;
}

/*
APP销注，如果APP不再使用ALARM功能，则需要应用这个函数销注。 
输入：appName--应用程序标识 
输出：TRUE--成功；FALSE--失败 
*/
char ALM_DeRegisterApp(const char *appName)
{
	return 0;
}

/*
系统调用，系统会检测到RTC产生中断时，调用这个函数。获取当前响闹ALARM信息和个数 
输入：pAlmInfo--传给系统的目的ALARM信息数组， 
      pAlmCount--数组中元素个数， 
	  maxRequest--最大需求个数 
输出：TRUE--成功；FALSE--失败 
*/
BYTE ALM_GetAlarmInfo(ALM_CINFO_T *pAlmInfo, WORD *pAlmCount, WORD maxRequest)
{
	return 0;
}

/*
查看ALARM项下次工作的时间 
输入：pInDT--要查看的时间指针；pOutDT--转换后时间指针；itemType--要查看的ALARM事件类型；interval--固定间隔时间长度（如果不是固定间隔类型则为零） 
输出：TRUE--成功；FALSE--没有相应时间 
*/
char ALM_TestResponse(SYSTEMTIME *pInDT, SYSTEMTIME *pOutDT, char itemType, DWORD interval)
{
	return 0;
} 

/*
增加ALARM事件，当增加一个ALARM项后，在该项生命期内，编号不会改变。 
输入：pAlmItem--要增加的ALARM事件结构指针 
输出：ALARM事件序号 
*/
DWORD ALM_AddAlarm(ALM_ITEM_T *pAlmItem) 
{
	return 0;
}

/*
删除ALARM事件 
输入：itemNo--要删除的ALARM事件序号 
输出：ALARM事件序号 
*/
DWORD ALM_DelAlarm(DWORD itemNo)
{
	return 0;
}

/*
修改ALARM事件
输入：almItem--要修改的ALARM事件结构指针；itemNo--要修改的ALARM事件序号
输出：ALARM事件序号 
*/
DWORD ALM_ModifyAlarm(ALM_ITEM_T *pAlmItem, DWORD itemNo)
{
	return 0;
}

/*
重新标记ALARM事件，用于系统崩溃后重建ALARM
输入：pAlmItem--要修改的ALARM事件结构指针；itemNo--要修改的ALARM事件序号
输出：TRUE--成功；FALSE--失败
*/
BYTE ALM_RemarkAlmItem(ALM_ITEM_T *pAlmItem, DWORD itemNo)
{
	return 0;
}

/**************************************************************/
static U8 AlarmOn = 0;
static void (*AlarmCallBack)(void);

static void __irq IsrAlarm(void)
{
//	WrUTXH0('A');
	if(AlarmCallBack)
		(*AlarmCallBack)();
	ClearPending(BIT_RTC);
}

void OpenAlarm(void (*cbf)(void))
{
	AlarmOn = 1;
	rRTCALM = (0x7f);	//enable alarm
	//唯一指定的时间,年/月/日/时/分/秒方式只触发一次
	//若要按年/季/月/时/分固定间隔重触发需要再设alarm的date,time
	AlarmCallBack = cbf;
	pISR_RTC = (U32)IsrAlarm;
	ClearPending(BIT_RTC);	
	EnableIrq(BIT_RTC);		
}

void CloseAlarm(void)
{
	AlarmOn = 0;
	rRTCALM = 0;			//disable alarm
	AlarmCallBack = NULL;
	DisableIrq(BIT_RTC);
}

U8 AlarmSetDate(DATETIME_T *date)
{
	if(AlarmOn&&RtcCheckDateValid(date->year, date->month, date->day))
	{
		U8 tmp;
		
		if((date->year<2000)||(date->year>2999))
			return FALSE;
		if((date->hour>23)||(date->minute>59)||(date->second>59))
			return FALSE;
		
		rRTCCON = 1;	//enable RTC write;
		
		tmp = date->second;
		rALMSEC  = ((tmp/10)<<4)+(tmp%10);
		tmp = date->minute;
		rALMMIN  = ((tmp/10)<<4)+(tmp%10);
		tmp = date->hour;
		rALMHOUR = ((tmp/10)<<4)+(tmp%10);
		tmp = date->day;
		rALMDATE = ((tmp/10)<<4)+(tmp%10);					
		tmp = date->month;
		rALMMON  = ((tmp/10)<<4)+(tmp%10);	
		tmp = date->year-2000;
		rALMYEAR = ((tmp/10)<<4)+(tmp%10);
		
		rRTCCON = 0;	//disable RTC write
			
		return TRUE;	
	}
	return FALSE;
}

U8 AlarmGetDate(DATETIME_T *date)
{
	if(AlarmOn)
	{
		U8 tmp;
		
		tmp = rALMYEAR;
		date->year     = (tmp>>4)*10+(tmp&0xf);
		tmp = rALMMON;
		date->month    = (tmp>>4)*10+(tmp&0xf);		
		tmp = rALMDATE;
		date->day      = (tmp>>4)*10+(tmp&0xf);
		tmp = rALMHOUR;
		date->hour     = (tmp>>4)*10+(tmp&0xf);
		tmp = rALMMIN;
		date->minute   = (tmp>>4)*10+(tmp&0xf);
		tmp = rALMSEC;					
		date->second   = (tmp>>4)*10+(tmp&0xf);
	
		return TRUE;
	}
	return FALSE;
}

/*
ALARM触发后，通过下列接口通知OS，OS进行消息封装后提供给给GUI或应用层
输入：没有
返回：没有
注意：AlarmProc（）函数只能在高级中HISR中被调用，否则系统异常。
*/
void AlarmProc(void)
{
}