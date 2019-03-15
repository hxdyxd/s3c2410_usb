#include "def.h"
#include "2410addr.h"
#include "rtcapi.h"
#include "2410lib.h"

#define	TotSecPerDay		86400
#define	TotDaysPerYear		365
#define	TotDaysPer4Year		1461
#define	TotDaysPer100Year	36524
#define	TotDaysPer400Year	146097
#define	GetWeekFromTotDays(days)	(((days)+0)%7)		//1��1��1��(��1��)����
#define	GetWeek(year, month, day)	(GetWeekFromTotDays(GetTotDays(year, month, day)))	

static U8 monthday[]={31,28,31,30,31,30,31,31,30,31,30,31};

/*************************************************************/
static U32 GetTotDays(U16 year, U16 month, U16 day)
{
	U16 y, y1, y2, y3;
	U32 d=0;	
/*			
	y = year+3;		
	y1 = y/400;
	y2 = (y%400)/100;
	y3 = ((y%400)%100)/4;
	y4 = year-y1*97-y2*24-y3;
	d  = (y1*97+y2*24+(y2?1:0)+y3)*366+y4*365;
*/	
	y  = year-1;	//passed year
	y1 = y/400;
	y2 = (y%400)/100;
	y3 = ((y%400)%100)/4;
	y  = y-y1*97-y2*24-y3;	 
	d  = (y1*97+y2*24+y3)*366+y*365;
	
//		printf("y1 = %d, y2= %d, y3 = %d, %d\r\n", y1, y2, y3, d);	
	
	for(y1=1; y1<month; y1++)
		d += monthday[y1-1];
	if(RtcIsLeapYear(year)&&(month>2))		
		d++;
//		printf("Total days = %d\r\n", d+day);
//		getch();	
	return d+day;
}

static void TotDaysToDate(U32 days, DATE_T *date)
{
	U16 y1, y2, y3, y4;
	
	days--;
	y1 = (days/TotDaysPer400Year);
	y2 = ((days%TotDaysPer400Year)/TotDaysPer100Year);
	y3 = (((days%TotDaysPer400Year)%TotDaysPer100Year)/TotDaysPer4Year);
	y4 = (((days%TotDaysPer400Year)%TotDaysPer100Year)%TotDaysPer4Year);
	
	y1 = y1*400+y2*100+y3*4+y4/TotDaysPerYear+1;
	y3 = y4%TotDaysPerYear;
	if(y4==(TotDaysPer4Year-1))
	{
		y1--;
		y3 = TotDaysPerYear;
	}
	if(!((days+1)%TotDaysPer400Year))
	{
		y1--;
		y3 = TotDaysPerYear;
	}
	y2 = 0;
	y3++;		
		
	while(1)
	{
		y4 = monthday[y2++];
		if(RtcIsLeapYear(y1)&&(y2==2))
			y4++;
		if(y3<=y4)
			break;
		y3 -= y4;					
	}
	
	date->year  = y1;
	date->month = y2;
	date->day   = y3;
	date->week  = GetWeekFromTotDays(days+1);
}
/*
void TestRtcFunc(void)
{
	U16 y, m , d, week, MaxDay;
	U32 day, err;
	DATE_T date;	
	
	day = 1;
	err = 0;
	for(y=1; y; y++)
	{
					
		for(m=1; m<13; m++)
		{
			MaxDay = RtcGetDaysOfMonth(y, m);
			for(d=1; d<=MaxDay; d++)
			{											
			//	if(day!=GetTotDays(y, m, d);)
			//	{
			//		printf("error!%d-%d-%d\r\n", y, m, d);					
			//		err++;					
			//	}					
			
				
			//	week = GetWeekFromTotDays(day);								
			//	TotDaysToDate(day, &date);
			//	if((date.year!=y)||(date.month!=m)||(date.day!=d)||(date.week!=week))
			//	{
			//		printf("error!%d-%d-%d, %d-%d-%d\r\n", y, m, d, date.year, date.month, date.day);
			//		err++;					
			//	}
			//	//else
			//	//	printf("%d-%d-%d,week%d\r\n", y, m, d, week);				
			//	if(getkey())
			//	{
			//		if(getch()==0x1b)
			//			goto exit_test;
			//	}
			
			//	if(!RtcCheckDateValid(y, m, d))
			//		printf("Date valid%d\r\n", ++err);
			//	if(RtcGetWeekNo(y, m, d)!=GetWeekFromTotDays(day))
			//		printf("week error\r\n");				
			
			//	if(day!=RtcGetPastDays(y, m, d))
			//		printf("err!\r\n");
			//	if((m==12)&&(d==31))
			//		day = 0;
					
				day++;
			}						
		}
		
		
		{
		//	DATE_T date1, date2;
		//	date1.year=y;
		//	date1.month=3;
		//	date1.day=1;
		//	date2.year=y+1;
		//	date2.month=3;
		//	date2.day=1;
		//	printf("%d-%d-%d, %d-%d-%d : %d\r\n",date1.year,date1.month,
		//		date1.day, date2.year,date2.month,date2.day,RtcGetDateInterval(&date1, &date2));
		}
		
	}
exit_test:
	if(!err)
		puts("All day OK!\r\n");
}
*/
/*************************************************************/
/*
���ܣ��ж�ĳ���ǲ������ꣻ
���룺�ꣻ
���أ�TRUE��FALSE��
˵��������Ϊ������  
*/
U8 RtcIsLeapYear(U16 year)
{
	return ((year%4==0)&&(year%100!=0)||(year%400==0));	
}

/*
���ܣ�������ڵ���Ч�ԣ�
���룺�꣬�£��գ�
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊ����
*/
U8 RtcCheckDateValid(U16 year, U16 month, U16 day)
{
	U8 MaxDay;
	
	if(!year)
		return FALSE;
	
	if((month-1)>11)
		return FALSE;
	
	if(RtcIsLeapYear(year)&&(month==2))
		MaxDay = 28;
	else
		MaxDay = monthday[month-1]-1;
	
	if((day-1)>MaxDay)
		return FALSE;	
				
	return TRUE;
}

/************************************************************/
#define	EnRtcWr()	(rRTCCON |= 1)
#define	DsRtcWr()	(rRTCCON &= ~1)

int RtcGetDate(DATETIME_T *date)
{
	U8 tmp, sec;
	
ReadRtc:
	tmp = rBCDDAY;
	if(tmp==7)
		date->week = 0;
	else
		date->week = tmp;
		
	sec = rBCDSEC;
				
	tmp = rBCDYEAR;
	date->year     = (tmp>>4)*10+(tmp&0xf);
	tmp = rBCDMON;
	date->month    = (tmp>>4)*10+(tmp&0xf);		
	tmp = rBCDDATE;
	date->day      = (tmp>>4)*10+(tmp&0xf);
	tmp = rBCDHOUR;
	date->hour     = (tmp>>4)*10+(tmp&0xf);
	tmp = rBCDMIN;
	date->minute   = (tmp>>4)*10+(tmp&0xf);
	tmp = rBCDSEC;
	if(tmp!=sec)
		goto ReadRtc;			
	date->second   = (tmp>>4)*10+(tmp&0xf);		
	
	date->millisecond = 0;
	
	return TRUE;
}

int RtcSetDate(DATETIME_T *date)
{	
	if(RtcCheckDateValid(date->year, date->month, date->day))
	{
		U16 tmp, week;
		
		if((date->year<2000)||(date->year>2999))
			return FALSE;			
		if((date->hour>23)||(date->minute>59)||(date->second>59))
			return FALSE;		

		week = GetWeek(date->year, date->month, date->day);
	
		//reset RTC clock count and enable write
		rRTCCON = 9;//EnRtcWr();
	
		tmp = date->second;
		rBCDSEC  = ((tmp/10)<<4)+(tmp%10);
		tmp = date->minute;
		rBCDMIN  = ((tmp/10)<<4)+(tmp%10);
		tmp = date->hour;
		rBCDHOUR = ((tmp/10)<<4)+(tmp%10);
		tmp = date->day;
		rBCDDATE = ((tmp/10)<<4)+(tmp%10);					
		tmp = date->month;
		rBCDMON  = ((tmp/10)<<4)+(tmp%10);	
		tmp = date->year-2000;
		rBCDYEAR = ((tmp/10)<<4)+(tmp%10);
	
		if(!week)
			rBCDDAY = 7;
		else	
			rBCDDAY = week;		
	
		//no reset RTC clock and disable write
		rRTCCON = 0;//DsRtcWr();	
	
		return TRUE;
	}
	return FALSE;	
}

/*
RTC��ʱ���֡����жϴ�����ͨ�����нӿ�֪ͨOS��OS������Ϣ��װ���ṩ����GUI��Ӧ�ò�
���룺type ��ʶ����ʱ���֡��룬���庬���������£�
1 �D ʱ, 2 �D ��, 3 �D ��
���أ�û��
ע�⣺RtcMsgProc��������ֻ���ڸ߼���HISR�б����ã�����ϵͳ�쳣��
*/
void RtcMsgProc(U8 type)
{
}

/*
���ܣ�����ĳһ�������ڼ���
���룺�꣬�£��գ�
���أ��ܺţ�
˵�����ܷ�Χ[0��6]��0�������죬����Ϊ������  
*/
U8 RtcGetWeekNo(U16 year, U16 month, U16 day)
{
	if(!RtcCheckDateValid(year, month, day))
		return 0xff;	
	
	return GetWeek(year, month, day);
}

/*
���ܣ�����ĳ�����ж����죻
���룺�꣬�£�
���أ�������
˵��������Ϊ������
*/
U8 RtcGetDaysOfMonth(U16 year, U16 month)
{
	if((month-1)>11)
		return 0xff;
	if(RtcIsLeapYear(year)&&(month==2))
		return 29;
	return monthday[month-1];			
}

/*
���ܣ��Ƚ���������ʱ����Ⱥ� 
���룺�������ʱ��ṹ��ָ�룻 
���أ�1--date1>date2,0--date1=date2,-1--date1<date2,���ڷǷ� 0xf0
˵��������Ϊ������
*/
char RtcCmpDateTime(DATETIME_T *date1, DATETIME_T *date2) 
{
	if(RtcCheckDateValid(date1->year, date1->month, date1->day)&&
		RtcCheckDateValid(date2->year, date2->month, date2->day))
	{
		U32 d1, d2;
		
		d1 = (date1->year<<16)+(date1->month<<8)+date1->day;
		d2 = (date2->year<<16)+(date2->month<<8)+date2->day;
		if(d1!=d2)
			return (d1>d2)?1:-1;
		d1 = (date1->hour<<16)+(date1->minute<<8)+date1->second;
		d2 = (date2->hour<<16)+(date2->minute<<8)+date2->second;
		if(d1!=d2) 
			return (d1>d2)?1:-1;
		
		return  0;			
	}
	return 0xf0;
}

/*
���ܣ�������������֮��ļ��������
���룺������ڽṹ��ָ�룻
���أ���������֮��ļ��������
˵��������Ϊ������  
*/
U32 RtcGetDateInterval(DATE_T *date1, DATE_T *date2)
{
	if(RtcCheckDateValid(date1->year, date1->month, date1->day)&&
		RtcCheckDateValid(date2->year, date2->month, date2->day))
	{	
		U32	d1, d2;
	
		d1 = GetTotDays(date1->year, date1->month, date1->day);
		d2 = GetTotDays(date2->year, date2->month, date2->day);
		
		return (d1>d2)?(d1-d2):(d2-d1);			
	}
	return 0xffffffff;
}
 
/*
���ܣ�������������֮��ļ��������
���룺������ڽṹ��ָ�룻
���أ���������֮��ļ����������
˵��������Ϊ����
*/
U32 RtcGetTimeInterval(DATETIME_T* pTime1, DATETIME_T *pTime2)
{
	if(RtcCheckDateValid(pTime1->year, pTime1->month, pTime1->day)&&
		RtcCheckDateValid(pTime2->year, pTime2->month, pTime2->day))
	{
		U32 d1, d2, s1, s2;
	
		d1 = GetTotDays(pTime1->year, pTime1->month, pTime1->day);
		d2 = GetTotDays(pTime2->year, pTime2->month, pTime2->day);	
		s1 = (pTime1->hour*60+pTime1->minute)*60+pTime1->second;
		s2 = (pTime2->hour*60+pTime2->minute)*60+pTime2->second;
	
		if(d1==d2)	
			return (s1>s2)?(s1-s2):(s2-s1);		
		
		if(d1>d2)
			return (d1-d2)*TotSecPerDay-s2+s1; 	
		else
			return (d2-d1)*TotSecPerDay-s1+s2;
	}
	return 0xffffffff;
}

/*
���ܣ�����ĳһ�����ڵ����������е�λ�ã�
���룺�꣬�£��գ�
���أ�ĳһ�����ڵ����������е�λ�ã�
˵��������Ϊ������  
*/
U16 RtcGetPastDays(U16 year, U16 month, U16 day)
{
	if(RtcCheckDateValid(year, month, day))
	{
		U16 i, d=day;	
		
		for(i=1; i<month; i++)
			d += monthday[i-1];					
		if(RtcIsLeapYear(year)&&(month>2))
			d++;
		return d;			
	}
	return 0xffff;	
}

/*
���ܣ�����ĳ�����ڼӶ������������ڣ�
���룺������ڽṹ��ָ��,�ӵ�������
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊ������  
*/
U8 RtcDateAddDays(DATE_T *date, U16 days)
{
	if(RtcCheckDateValid(date->year, date->month, date->day))
	{
		U32 d;	
	
		d  = GetTotDays(date->year, date->month, date->day);			
		TotDaysToDate(d+days, date);			
		return TRUE;
	}
	return FALSE;	
}

/*
���ܣ�����ĳ�����ڼ��������������ڣ�
���룺������ڽṹ��ָ��,����������
���أ�TRUE or FALSE;
˵��������Ϊ������  
*/
U8 RtcDateSubDays(DATE_T *date, U16 days)
{
	if(RtcCheckDateValid(date->year, date->month, date->day))
	{
		U32 d;	
	
		d  = GetTotDays(date->year, date->month, date->day);
		if(d<=days)
			return FALSE;	
		TotDaysToDate(d-days, date);
		return TRUE;
	}
	return FALSE;
}

/*
���ܣ������¸��µ����ڣ�
���룺�������������ڽṹ�ĵ�ַ��
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊ������  
*/
U8 RtcGetNextMonth(DATE_T *date)
{
	return RtcDateAddDays(date, 30);
}

/*
���ܣ������ϸ��µ����ڣ�
���룺�������������ڽṹ�ĵ�ַ��
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊ������  
*/
U8 RtcGetPreMonth(DATE_T *date)
{
	return RtcDateSubDays(date, 30);
}

/*
���ܣ�������һ������ڣ�
���룺�������������ڽṹ�ĵ�ַ;
���أ�TRUE --������ȷ,FALSE--���ڴ���;
˵��������Ϊ���� 
*/
U8 RtcGetNextYear(DATE_T *date)
{
	return RtcDateAddDays(date, TotDaysPerYear);
}

/*
���ܣ�������һ������ڣ�
���룺�������������ڽṹ�ĵ�ַ��
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊ������
*/
U8 RtcGetPreYear(DATE_T *date)
{
	return RtcDateSubDays(date, TotDaysPerYear);	
}

/***********************************************************/
/*
���ܣ�����ĳһ���ũ����Ф��
���룺date--����������ڽṹ�ĵ�ַ��
	  solar--1==��������������,0==����������ũ��
���أ�0~11
˵����0��ʾ���ꣻ
*/
U8  RtcGetAnimalYear(DATE_T *date, U8 solar)
{
	return 0;
}

/*
���ܣ�����תũ����
���룺src_date--�������������ڽṹ��ָ�룻
���أ�0 --�����ũ���Ƿ�����,1--�����ũ��������
˵����û�У�  
*/
U8 RtcSolarConvertLunar(DATE_T *src_date)
{
	return 0;
} 

/*
���ܣ�����ũ��ĳ�����ж����죻
���룺�꣬�£�
���أ�TRUE --������ȷ,FALSE--���ڴ���
˵��������Ϊũ����  
*/
U8 RtcLunarDayOfMonth(U16 year, U16 month)
{
	return 0;
}

/*
���ܣ�ũ��ת������
���룺src_date--�������������ڽṹ��ָ��,dec_date---�����������������������Ǵ�ŵڶ��������ĵ�ַ��
���أ�0 --�����£�ֻ���һ������,1--���£������������,0xff--���ڴ���
˵�������ũ����Ϊ���£���Ӧ���������������ڣ�
*/
U8 RtcLunarConvertSolar(DATE_T *src_date, DATE_T *dec_date)
{
	return 0;
} 
