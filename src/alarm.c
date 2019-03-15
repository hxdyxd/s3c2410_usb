#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "rtcapi.h"
#include "alarm.h"

/*
ALARM��ʼ������ϵͳ��һ�����л��߸�λʱ���С���������ALARM���ݲ�����ALARM�� 
���룺�� 
�����TRUE--�ɹ���FALSE--ʧ��
*/
char ALM_Init(void)
{
	return 0;
}

/*
APPע�ᣬ���APPҪʹ��ALARM���ܣ��������Ӧ���������ע�ᡣ 
���룺pAlmApp--Ҫ���ӵ�APPע����ṹָ�� 
�����TRUE--�ɹ���FALSE--ʧ�� 
*/
char ALM_RegisterApp(ALM_APP_T almApp)
{
	return 0;
}

/*
APP��ע�����APP����ʹ��ALARM���ܣ�����ҪӦ�����������ע�� 
���룺appName--Ӧ�ó����ʶ 
�����TRUE--�ɹ���FALSE--ʧ�� 
*/
char ALM_DeRegisterApp(const char *appName)
{
	return 0;
}

/*
ϵͳ���ã�ϵͳ���⵽RTC�����ж�ʱ�����������������ȡ��ǰ����ALARM��Ϣ�͸��� 
���룺pAlmInfo--����ϵͳ��Ŀ��ALARM��Ϣ���飬 
      pAlmCount--������Ԫ�ظ����� 
	  maxRequest--���������� 
�����TRUE--�ɹ���FALSE--ʧ�� 
*/
BYTE ALM_GetAlarmInfo(ALM_CINFO_T *pAlmInfo, WORD *pAlmCount, WORD maxRequest)
{
	return 0;
}

/*
�鿴ALARM���´ι�����ʱ�� 
���룺pInDT--Ҫ�鿴��ʱ��ָ�룻pOutDT--ת����ʱ��ָ�룻itemType--Ҫ�鿴��ALARM�¼����ͣ�interval--�̶����ʱ�䳤�ȣ�������ǹ̶����������Ϊ�㣩 
�����TRUE--�ɹ���FALSE--û����Ӧʱ�� 
*/
char ALM_TestResponse(SYSTEMTIME *pInDT, SYSTEMTIME *pOutDT, char itemType, DWORD interval)
{
	return 0;
} 

/*
����ALARM�¼���������һ��ALARM����ڸ����������ڣ���Ų���ı䡣 
���룺pAlmItem--Ҫ���ӵ�ALARM�¼��ṹָ�� 
�����ALARM�¼���� 
*/
DWORD ALM_AddAlarm(ALM_ITEM_T *pAlmItem) 
{
	return 0;
}

/*
ɾ��ALARM�¼� 
���룺itemNo--Ҫɾ����ALARM�¼���� 
�����ALARM�¼���� 
*/
DWORD ALM_DelAlarm(DWORD itemNo)
{
	return 0;
}

/*
�޸�ALARM�¼�
���룺almItem--Ҫ�޸ĵ�ALARM�¼��ṹָ�룻itemNo--Ҫ�޸ĵ�ALARM�¼����
�����ALARM�¼���� 
*/
DWORD ALM_ModifyAlarm(ALM_ITEM_T *pAlmItem, DWORD itemNo)
{
	return 0;
}

/*
���±��ALARM�¼�������ϵͳ�������ؽ�ALARM
���룺pAlmItem--Ҫ�޸ĵ�ALARM�¼��ṹָ�룻itemNo--Ҫ�޸ĵ�ALARM�¼����
�����TRUE--�ɹ���FALSE--ʧ��
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
	//Ψһָ����ʱ��,��/��/��/ʱ/��/�뷽ʽֻ����һ��
	//��Ҫ����/��/��/ʱ/�̶ֹ�����ش�����Ҫ����alarm��date,time
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
ALARM������ͨ�����нӿ�֪ͨOS��OS������Ϣ��װ���ṩ����GUI��Ӧ�ò�
���룺û��
���أ�û��
ע�⣺AlarmProc��������ֻ���ڸ߼���HISR�б����ã�����ϵͳ�쳣��
*/
void AlarmProc(void)
{
}