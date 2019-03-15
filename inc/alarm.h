#ifndef	_ALARM_H
#define	_ALARM_H

#define	ALM_APP_NAME_LEN	256

#define	ALM_INVALID		0	/* ȡ��״̬  */ 
#define	ALM_ONCE		1	/* һ����    */ 
#define	ALM_DAYS		2	/* ÿ��      */ 
#define	ALM_WEEKLY		3	/* ÿ��      */ 
#define	ALM_MONTHLY		4	/* ÿ��      */ 
#define	ALM_QUARTERLY	5	/* ÿ����    */
#define	ALM_YEARLY		6	/* ÿ��      */ 
#define	ALM_FIXEDTICKS	7	/* �̶����  */

typedef struct _alarm_app_{ 
	char	AppName[ALM_APP_NAME_LEN];	/* Ӧ�ó���ı�ʶ */
	void	(*Rebuild)(void);			/* ��λʱ����ALARM��Ļص����� */ 
}ALM_APP_T; 

typedef struct _cur_alarm_info{ 
	char 	AppName[ALM_APP_NAME_LEN];	/* Ӧ�ó���ı�ʶ */
	DWORD	UserData;					/* �û����� */ 
} ALM_CINFO_T;

typedef struct _alarm_item_{ 
	char 	AppName[ALM_APP_NAME_LEN];	/* Ӧ�ó���ı�ʶ */
	BYTE 	ItemType;					/* ALARM ���ͣ�һ��\ÿ��\ÿ�¡�����*/ 
	DWORD 	Interval;					/* �̶�������͵�ʱ��������λ���֣�*/ 
	SYSTEMTIME DateTime;				/* ALARM����ʱ�� */ 
	DWORD 	UserData;					/* �û����� */ 
}ALM_ITEM_T;

char ALM_Init(void);
char ALM_RegisterApp(ALM_APP_T almApp); 
char ALM_DeRegisterApp(const char *appName);
BYTE ALM_GetAlarmInfo(ALM_CINFO_T *pAlmInfo, WORD *pAlmCount, WORD maxRequest);
char ALM_TestResponse(SYSTEMTIME *pInDT,  SYSTEMTIME *pOutDT, char itemType, DWORD interval); 
DWORD ALM_AddAlarm(ALM_ITEM_T *pAlmItem);
DWORD ALM_DelAlarm(DWORD itemNo);
DWORD ALM_ModifyAlarm(ALM_ITEM_T *pAlmItem, DWORD itemNo);
BYTE ALM_RemarkAlmItem(ALM_ITEM_T *pAlmItem, DWORD itemNo);

void OpenAlarm(void (*cbf)(void));
void CloseAlarm(void);
U8 AlarmSetDate(DATETIME_T *date);
U8 AlarmGetDate(DATETIME_T *date);
void AlarmProc(void);

#endif	/* _ALARM_H */