/**************************************************************
The initial and control for 320×240 16Bpp TFT LCD----3.5寸竖屏
**************************************************************/

#include <string.h>
#include "def.h"
#include "2410addr.h"
#include "2410slib.h"
#include "2410lib.h"
#include "LCD_TX06D18.h"

#define MVAL		(13)
#define MVAL_USED 	(0)		//0=each frame   1=rate by MVAL
#define INVVDEN		(1)		//0=normal       1=inverted
#define BSWP		(0)		//Byte swap control
#define HWSWP		(1)		//Half word swap control

#define M5D(n) ((n) & 0x1fffff)	// To get lower 21bits

//TFT 240320
#define LCD_XSIZE_TFT_240320 	(240)	
#define LCD_YSIZE_TFT_240320 	(320)

//TFT 240320
#define SCR_XSIZE_TFT_240320 	(640)
#define SCR_YSIZE_TFT_240320 	(480)
//#define SCR_XSIZE_TFT_240320 	(240)
//#define SCR_YSIZE_TFT_240320 	(320)

//TFT240320
#define HOZVAL_TFT_240320	(LCD_XSIZE_TFT_240320-1)
#define LINEVAL_TFT_240320	(LCD_YSIZE_TFT_240320-1)

//Timing parameter for LCD
#define VBPD_240320		(2)		//垂直同步信号的后肩
#define VFPD_240320		(2)		//垂直同步信号的前肩
#define VSPW_240320		(4)		//垂直同步信号的脉宽

#define HBPD_240320		(8)		//水平同步信号的后肩
#define HFPD_240320		(8)		//水平同步信号的前肩
#define HSPW_240320		(6)		//水平同步信号的脉宽

#define CLKVAL_TFT_240320	(5) 	
//FCLK=180MHz,HCLK=90MHz,VCLK=6.5MHz

static void Lcd_Init(void);
static void Lcd_EnvidOnOff(int onoff);
//static void Lcd_Lpc3600Enable(void);
//static void Lcd_PowerEnable(int invpwren,int pwren);
static void MoveViewPort(void);
static void Lcd_MoveViewPort(int vx,int vy);
static void PutPixel(U32 x,U32 y,U32 c);
//static void Glib_Rectangle(int x1,int y1,int x2,int y2,int color);
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color);
static void Glib_Line(int x1,int y1,int x2,int y2,int color);
static void Lcd_ClearScr(U16 c);
static void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[]);

//extern unsigned char girl13_240_320[];	//宽240，高320
extern unsigned char xyx_240_320[];	//宽240，高320

volatile static unsigned short LCD_BUFER[SCR_YSIZE_TFT_240320][SCR_XSIZE_TFT_240320];

/**************************************************************
320×240 16Bpp TFT LCD数据和控制端口初始化
**************************************************************/
static void Lcd_Port_Init(void)
{
    //rGPCUP = 0xffffffff; // Disable Pull-up register
    rGPCUP = 0x0; // enable Pull-up register
    rGPCCON = 0xaaaa56a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND 

    //rGPDUP = 0xffffffff ; // Disable Pull-up register
    rGPDUP = 0x0 ; // enable Pull-up register
    rGPDCON=0xaaaaaaaa; //Initialize VD[15:8]
}

/**************************************************************
320×240 16Bpp TFT LCD功能模块初始化
**************************************************************/
static void Lcd_Init(void)
{
	rLCDCON1=(CLKVAL_TFT_240320<<8)|(MVAL_USED<<7)|(3<<5)|(12<<1)|0;
    	// TFT LCD panel,12bpp TFT,ENVID=off
	rLCDCON2=(VBPD_240320<<24)|(LINEVAL_TFT_240320<<14)|(VFPD_240320<<6)|(VSPW_240320);
	rLCDCON3=(HBPD_240320<<19)|(HOZVAL_TFT_240320<<8)|(HFPD_240320);
	rLCDCON4=(MVAL<<8)|(HSPW_240320);
	rLCDCON5=(1<<11)|(0<<9)|(0<<8)|(0<<6)|(BSWP<<1)|(HWSWP);	//FRM5:6:5,HSYNC and VSYNC are inverted----LQ035Q7DB02
	
	rLCDSADDR1=(((U32)LCD_BUFER>>22)<<21)|M5D((U32)LCD_BUFER>>1);
	rLCDSADDR2=M5D( ((U32)LCD_BUFER+(SCR_XSIZE_TFT_240320*LCD_YSIZE_TFT_240320*2))>>1 );
	rLCDSADDR3=(((SCR_XSIZE_TFT_240320-LCD_XSIZE_TFT_240320)/1)<<11)|(LCD_XSIZE_TFT_240320/1);
	
	rLCDINTMSK|=(3); // MASK LCD Sub Interrupt
	rLPCSEL&=(~7); // Disable LPC3600
	rTPAL=0; // Disable Temp Palette
}

/**************************************************************
LCD视频和控制信号输出或者停止，1开启视频输出
**************************************************************/
static void Lcd_EnvidOnOff(int onoff)
{
    if(onoff==1)
	rLCDCON1|=1; // ENVID=ON
    else
	rLCDCON1 =rLCDCON1 & 0x3fffe; // ENVID Off
}

/**************************************************************
LPC3600 is a timing control logic unit
**************************************************************/
/*
static void Lcd_Lpc3600Enable(void)
{
    rLPCSEL&=~(7);
    rLPCSEL|=(7); // 240320,Enable LPC3600
}    
*/
/**************************************************************
320×240 8Bpp TFT LCD 电源控制引脚使能
**************************************************************/
/*
static void Lcd_PowerEnable(int invpwren,int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP = rGPGUP|(1<<4); // Pull-up disable
    rGPGCON = rGPGCON|(3<<8); //GPG4=LCD_PWREN
    //Enable LCD POWER ENABLE Function
    rLCDCON5 = rLCDCON5&(~(1<<3))|(pwren<<3);   // PWREN
    rLCDCON5=rLCDCON5&(~(1<<5))|(invpwren<<5);   // INVPWREN
}
*/
/**************************************************************
320×240 8Bpp TFT LCD 颜色初始化
**************************************************************/
/*
static void Lcd_Palette_Init(void)
{
    unsigned char cdata, p_red, p_green, p_blue;
    U32 *palette;
    
	//#define PALETTE     0x4d000400    //Palette start address
    palette=(U32 *)PALETTE;
    *palette++=0; //black
    for(cdata=1;cdata<255;cdata++)
    {
		p_red=(cdata & 0xe0);
		p_green=(cdata & 0x1c);
		p_blue=(cdata & 0x03);
    	*palette++=((U32)((p_red<<8)|(p_green<<6)|(p_blue<<3)));
    }
    *palette=0xffff; //white
}
*/
/**************************************************************
320×240 16Bpp TFT LCD移动观察窗口
**************************************************************/
static void Lcd_MoveViewPort(int vx,int vy)
{
    U32 addr;

    SET_IF(); 
	#if (LCD_XSIZE_TFT_240320<32)
    	    while((rLCDCON1>>18)<=1); // if x<32
	#else	
    	    while((rLCDCON1>>18)==0); // if x>32
	#endif
    
    addr=(U32)LCD_BUFER+(vx*2)+vy*(SCR_XSIZE_TFT_240320*2);
	rLCDSADDR1= ( (addr>>22)<<21 ) | M5D(addr>>1 );
	rLCDSADDR2= M5D(((addr+(SCR_XSIZE_TFT_240320*LCD_YSIZE_TFT_240320*2))>>1));
	CLR_IF();
}    

/**************************************************************
320×240 16Bpp TFT LCD移动观察窗口
**************************************************************/
static void MoveViewPort(void)
{
    int vx=0,vy=0,vd=1;

    printf("\r\n*Move the LCD view windos:\r\n");
    printf(" press 8 is up\r\n");
    printf(" press 2 is down\r\n");
    printf(" press 4 is left\r\n");
    printf(" press 6 is right\r\n");
    printf(" press Enter to exit!\r\n");

    while(1)
    {
    	switch(getkey())
    	{
    	case '8':
	    if(vy>=vd)vy-=vd;    	   	
        break;

    	case '4':
    	    if(vx>=vd)vx-=vd;
    	break;

    	case '6':
                if(vx<=(SCR_XSIZE_TFT_240320-LCD_XSIZE_TFT_240320-vd))vx+=vd;   	    
   	    break;

    	case '2':
                if(vy<=(SCR_YSIZE_TFT_240320-LCD_YSIZE_TFT_240320-vd))vy+=vd;   	    
   	    break;

    	case '\r':
   	    return;

    	default:
	    break;
		}
	//printf("vx=%3d,vy=%3d\r\n",vx,vy);
	Lcd_MoveViewPort(vx,vy);
    }
}

/**************************************************************
320×240 16Bpp TFT LCD单个象素的显示数据输出
**************************************************************/
static void PutPixel(U32 x,U32 y,U32 c)
{
	if ( (x < SCR_XSIZE_TFT_240320) && (y < SCR_YSIZE_TFT_240320) )
	LCD_BUFER[(y)][(x)] = c;
}

/**************************************************************
320×240 16Bpp TFT LCD全屏填充特定颜色单元或清屏
**************************************************************/
static void Lcd_ClearScr(U16 c)
{
	unsigned int x,y ;
		
    for( y = 0 ; y < SCR_YSIZE_TFT_240320 ; y++ )
    {
    	for( x = 0 ; x < SCR_XSIZE_TFT_240320 ; x++ )
    	{
			LCD_BUFER[y][x] = c;
    	}
    }
}

/**************************************************************
LCD屏幕显示垂直翻转
// LCD display is flipped vertically
// But, think the algorithm by mathematics point.
//   3I2
//   4 I 1
//  --+--   <-8 octants  mathematical cordinate
//   5 I 8
//   6I7
**************************************************************/
static void Glib_Line(int x1,int y1,int x2,int y2,int color)
{
	int dx,dy,e;
	dx=x2-x1; 
	dy=y2-y1;
    
	if(dx>=0)
	{
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 1/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 2/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 8/8 octant
			{
				e=dy-dx/2;
				while(x1<=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1+=1;
					e+=dy;
				}
			}
			else		// 7/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1+=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
	else //dx<0
	{
		dx=-dx;		//dx=abs(dx)
		if(dy >= 0) // dy>=0
		{
			if(dx>=dy) // 4/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1+=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 3/8 octant
			{
				e=dx-dy/2;
				while(y1<=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1+=1;
					e+=dx;
				}
			}
		}
		else		   // dy<0
		{
			dy=-dy;   // dy=abs(dy)

			if(dx>=dy) // 5/8 octant
			{
				e=dy-dx/2;
				while(x1>=x2)
				{
					PutPixel(x1,y1,color);
					if(e>0){y1-=1;e-=dx;}	
					x1-=1;
					e+=dy;
				}
			}
			else		// 6/8 octant
			{
				e=dx-dy/2;
				while(y1>=y2)
				{
					PutPixel(x1,y1,color);
					if(e>0){x1-=1;e-=dy;}	
					y1-=1;
					e+=dx;
				}
			}
		}	
	}
}

/**************************************************************
在LCD屏幕上画一个矩形
**************************************************************/
/*
static void Glib_Rectangle(int x1,int y1,int x2,int y2,int color)
{
    Glib_Line(x1,y1,x2,y1,color);
    Glib_Line(x2,y1,x2,y2,color);
    Glib_Line(x1,y2,x2,y2,color);
    Glib_Line(x1,y1,x1,y2,color);
}
*/
/**************************************************************
在LCD屏幕上用颜色填充一个矩形
**************************************************************/
static void Glib_FilledRectangle(int x1,int y1,int x2,int y2,int color)
{
    int i;

    for(i=y1;i<=y2;i++)
	Glib_Line(x1,i,x2,i,color);
}

/**************************************************************
在LCD屏幕上指定坐标点画一个指定大小的图片
**************************************************************/
static void Paint_Bmp(int x0,int y0,int h,int l,unsigned char bmp[])
{
	int x,y;
	U32 c;
	int p = 0;

//本图片显示是正的显示方式
    for( y = 0 ; y < l ; y++ )
    {
    	for( x = 0 ; x < h ; x++ )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;

			if ( ( (x0+x) < SCR_XSIZE_TFT_240320) && ( (y0+y) < SCR_YSIZE_TFT_240320) )
			LCD_BUFER[y0+y][x0+x] = c ;

    		p = p + 2 ;
    	}
    }

/*本图片显示是倒过来的，是旋转了180度的显示方式	
    for( y = l ; y > 0 ; y-- )
    {
    	for( x = h ; x > 0 ; x-- )
    	{
    		c = bmp[p+1] | (bmp[p]<<8) ;

			if ( ( (x0+x) < SCR_XSIZE_TFT_240320) && ( (y0+y) < SCR_YSIZE_TFT_240320) )
			LCD_BUFER[y0+y][x0+x] = c ;

    		p = p + 2 ;
    	}
    }
*/
}

void Lcd_Tft_240X320_Init( void )
{
    Lcd_Port_Init();
    Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio

	Lcd_ClearScr(0xffff);		//fill all screen with some color
	#define LCD_BLANK		16
	#define C_UP		( LCD_XSIZE_TFT_240320 - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE_TFT_240320 - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE_TFT_240320 - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE_TFT_240320 - LCD_BLANK ), ( LCD_YSIZE_TFT_240320 - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
}

/**************************************************************
**************************************************************/
void Test_Lcd_Tft_240X320( void )
{
	#ifdef DEBUG
    	Uart_Printf("\r\nTest 240*320 TFT LCD !\r\n");
	#endif	

    Lcd_Port_Init();
    Lcd_Init();
    Lcd_EnvidOnOff(1);		//turn on vedio
/*	
	Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x00)  )  ;		//clear screen
	Uart_Printf( "\r\nLCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x1f)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x00<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x00<<11) | (0x3f<<5) | (0x00)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x1f<<11) | (0x00<<5) | (0x00)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x00<<11) | (0x3f<<5) | (0x1f)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x1f<<11) | (0x00<<5) | (0x1f)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input

	Lcd_ClearScr( (0x1f<<11) | (0x3f<<5) | (0x00)  )  ;		//clear screen
	Uart_Printf( "LCD clear screen is finished! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input
*/
	Lcd_ClearScr(0xffff);		//fill all screen with some color
	#define C_UP		( LCD_XSIZE_TFT_240320 - LCD_BLANK*2 )
	#define C_RIGHT		( LCD_XSIZE_TFT_240320 - LCD_BLANK*2 )
	#define V_BLACK		( ( LCD_YSIZE_TFT_240320 - LCD_BLANK*4 ) / 6 )
	Glib_FilledRectangle( LCD_BLANK, LCD_BLANK, ( LCD_XSIZE_TFT_240320 - LCD_BLANK ), ( LCD_YSIZE_TFT_240320 - LCD_BLANK ),0x0000);		//fill a Rectangle with some color

	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*0), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*1),0x001f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*1), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*2),0x07e0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*2), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*3),0xf800);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*3), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*4),0xffe0);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*4), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*5),0xf81f);		//fill a Rectangle with some color
	Glib_FilledRectangle( (LCD_BLANK*2), (LCD_BLANK*2 + V_BLACK*5), (C_RIGHT), (LCD_BLANK*2 + V_BLACK*6),0x07ff);		//fill a Rectangle with some color
   	Uart_Printf( "LCD color test, please look! press any key to continue!\r\n" );
    Uart_Getch() ;		//wait uart input
/*
	Lcd_ClearScr(0x00);		//fill all screen with some color
    Paint_Bmp( 0,0,240,320, girl13_240_320 ) ;		//paint a bmp
    MoveViewPort();
    Lcd_MoveViewPort(0,0);
*/
	Lcd_ClearScr(0x00);		//fill all screen with some color
    Paint_Bmp( 0,0,240,320, xyx_240_320 ) ;		//paint a bmp
    MoveViewPort();
    Lcd_MoveViewPort(0,0);
}
//*************************************************************
