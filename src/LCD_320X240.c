/*
   copyright(c) 2006, ShenZhen uCdragon Technology Co., Ltd.
   All rights reserved.

   file:     lcd_320x240.c
   description: test Samsung's 3.5" Color TFT-LCD LTV350QV-F05.
               The initial and control, 16Bpp mode.
   platform: FS2410DEV V6.0
   creat:    ???
   modify:   zhongzm
   date:     2006/5/15 - ?
   version:  1.01
*/

#include <string.h>
#include "2410addr.h"
#include "2410lib.h"
#include "def.h"
#include "2410slib.h"


#define BSWP         (0)                  // Byte swap control
#define HWSWP        (1)                  // Half word swap control

#define M5D(n)       ((n) & 0x1fffff)     // get lower 21bits

// FCLK = 200MHz, HCLK = 100MHz
// VCLK = HCLK / [(CLKVAL+1) * 2]  (CLKVAL >= 0)
#define CLKVAL_TFT_320240       (7)

// TFT 320 x 240
#define LCD_XSIZE_TFT_320240    (320)
#define LCD_YSIZE_TFT_320240    (240)

// TFT 320 x 240
#define SCR_XSIZE_TFT_320240    (320)
#define SCR_YSIZE_TFT_320240    (240)

// TFT320 x 240
#define HOZVAL_TFT_320240       (LCD_XSIZE_TFT_320240 - 1)
#define LINEVAL_TFT_320240      (LCD_YSIZE_TFT_320240 - 1)

/*
 (1) Apply the following parameters to LTV350QV-F05 only!
 (2) LTV350QV-F05 works at SYNC mode, it use three control lines only: VCLK,
     HSYNC, VSYNC signals, the VDEN and LEND is not used. 
 (3) Setup timing parameter:
 

/********************************************************************
//东华屏:WXHAT35-TG2#001
#define VBPD_320240     (4)     //垂直同步信号的后肩
#define VFPD_320240     (4)     //垂直同步信号的前肩
#define VSPW_320240     (4)     //垂直同步信号的脉宽

#define HBPD_320240     (13)    //水平同步信号的后肩
#define HFPD_320240     (4)     //水平同步信号的前肩
#define HSPW_320240     (18)    //水平同步信号的脉宽
********************************************************************/

/********************************************************************/
//东华屏:WX3500B-M06
#define VBPD_320240     (14)     //垂直同步信号的后肩
#define VFPD_320240     (11)     //垂直同步信号的前肩
#define VSPW_320240     (2)     //垂直同步信号的脉宽

#define HBPD_320240     (37)    //水平同步信号的后肩
#define HFPD_320240     (19)     //水平同步信号的前肩
#define HSPW_320240     (29)    //水平同步信号的脉宽
/********************************************************************/
// GPB1/TOUT1 for Backlight control(PWM)
#define GPB1_TO_OUT()   (rGPBUP &= 0xfffd, rGPBCON &= 0xfffffff3, rGPBCON |= 0x00000004)
#define GPB1_TO_1()     (rGPBDAT |= 0x0002)
#define GPB1_TO_0()     (rGPBDAT &= 0xfffd)

volatile static unsigned short LCD_BUFER[SCR_YSIZE_TFT_320240][SCR_XSIZE_TFT_320240];


/*-------------------------- function declaration ---------------------------*/
static void Lcd_EnvidOnOff(int onoff);


/*-----------------------------------------------------------------------------
 *  LCD PWREN端口初始化, no use
 */
static void Lcd_Pwren_Init_On_Off( unsigned char m )
{
    rGPGCON = rGPGCON | (3<<8) ;
    rGPGUP = rGPGUP & ( ~(1<<4) ) ;
    if( m == TRUE )
        rGPGDAT = rGPGDAT | (1<<4) ;
    else
        rGPGDAT = rGPGDAT & ( ~(1<<4) );
}

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD数据和控制端口初始化
 */
void Lcd_Port_Init( void )
{
    //rGPCUP  = 0xffffffff; // Disable Pull-up register
    rGPCUP  = 0x00000000;
    //rGPCCON = 0xaaaa56a9; //Initialize VD[7:0],LCDVF[2:0],VM,VFRAME,VLINE,VCLK,LEND
    rGPCCON = 0xaaaa02a9;  //qjy: for avr config lcd!!!

    //rGPDUP  = 0xffffffff; // Disable Pull-up register
    rGPDUP  = 0x00000000;
    rGPDCON = 0xaaaaaaaa; //Initialize VD[15:8]
    
    Lcd_Pwren_Init_On_Off( TRUE ) ;
}

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD功能模块初始化
 */
void Lcd_Init(void)
{
    // TFT LCD panel, 16bpp TFT, ENVID=off
    rLCDCON1 = (CLKVAL_TFT_320240<<8) | (0<<7) | (3<<5) | (12<<1) | 0;
    rLCDCON2 = (VBPD_320240<<24) | (LINEVAL_TFT_320240<<14) | (VFPD_320240<<6)
             | (VSPW_320240);
    rLCDCON3 = (HBPD_320240<<19) | (HOZVAL_TFT_320240<<8) | (HFPD_320240);
    rLCDCON4 = (13<<8) | (HSPW_320240);
    rLCDCON5 = (1<<11) | (1<<10) | (1<<9) | (1<<8) | (0<<7) | (0<<6)
             | (1<<3)  |(BSWP<<1) | (HWSWP);

    rLCDSADDR1 = (((U32)LCD_BUFER>>22)<<21) | M5D((U32)LCD_BUFER>>1);
    rLCDSADDR2 = M5D( ((U32)LCD_BUFER
               +(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1 );
    rLCDSADDR3 = (((SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240)/1)<<11)
                |(LCD_XSIZE_TFT_320240/1);
    
    rLCDINTMSK |= (3);    // MASK LCD Sub Interrupt
    rLPCSEL &= (~7) ;     // Disable LPC3480
    rTPAL = 0 ;           // Disable Temp Palette
}

/*-----------------------------------------------------------------------------
 *  LCD视频和控制信号输出或者停止，1开启视频输出
 */
void Lcd_EnvidOnOff(int onoff)
{
    if(onoff == 1)
        rLCDCON1 |= 1;         // ENVID=ON
    else
        rLCDCON1 &= 0x3fffe;   // ENVID Off
}

/*-----------------------------------------------------------------------------
 *  LPC3600 is a timing control logic unit for LTS350Q1-PD1 or LTS350Q1-PD2
 *  no use
 */
static void Lcd_Lpc3600Enable(void)
{
    rLPCSEL &= ~(7);
    rLPCSEL |= (7);    // 320240, Enable LPC3480
}    

/*-----------------------------------------------------------------------------
 *  320 x 240 16Bpp TFT LCD 背光控制使能
 */
static void Lcd_Black_Light_On_Off( unsigned char m )
{
    rGPBCON = rGPBCON & (~(3<<10)) | (1<<10) ;    //GPB5 is output
    rGPBUP  = rGPBUP & (~(1<<5)) ;                 //GPB5 pull-up is enable
    
    if ( m > 0 ) rGPBDAT |= (1<<5) ;      //GPB5 output HIGH
    else rGPBDAT &= (~(1<<5));            //GPB5 output LOW
}    

/*-----------------------------------------------------------------------------
 *  320 x 240 16Bpp TFT LCD 电源控制引脚使能, no use
 */
static void Lcd_PowerEnable(int invpwren, int pwren)
{
    //GPG4 is setted as LCD_PWREN
    rGPGUP = rGPGUP | (1<<4);         // Pull-up disable
    rGPGCON = rGPGCON | (3<<8);       //GPG4=LCD_PWREN
    
    //Enable LCD POWER ENABLE Function
    rLCDCON5 = rLCDCON5 & (~(1<<3)) | (pwren<<3);      // PWREN
    rLCDCON5 = rLCDCON5 & (~(1<<5)) | (invpwren<<5);   // INVPWREN
}

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD移动观察窗口
 */
static void Lcd_MoveViewPort(int vx, int vy)
{
    U32 addr;

    SET_IF(); 
    #if (LCD_XSIZE_TFT_320240<32)
            while((rLCDCON1>>18)<=1);       // if x<32
    #else
            while((rLCDCON1>>18) == 0);     // if x>32
    #endif
    
    addr = (U32)LCD_BUFER+(vx*2)+vy*(SCR_XSIZE_TFT_320240*2);
    rLCDSADDR1= ( (addr>>22)<<21 ) | M5D(addr>>1);
    rLCDSADDR2= M5D(((addr+(SCR_XSIZE_TFT_320240*LCD_YSIZE_TFT_320240*2))>>1));
    CLR_IF();
}    

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD移动观察窗口
 */
static void MoveViewPort(void)
{
    int vx=0, vy=0, vd=1;

    Uart_Printf("\r\n*Move the LCD view windos:\r\n");
    Uart_Printf(" press 8 is up\r\n");
    Uart_Printf(" press 2 is down\r\n");
    Uart_Printf(" press 4 is left\r\n");
    Uart_Printf(" press 6 is right\r\n");
    Uart_Printf(" press Enter to exit!\r\n");

    while(1) {
        switch(Uart_Getch()) {
        case '8':
            if(vy >= vd) vy -= vd;
            break;
            
        case '4':
            if(vx>=vd)vx-=vd;
            break;
            
        case '6':
            if(vx<=(SCR_XSIZE_TFT_320240-LCD_XSIZE_TFT_320240-vd)) vx += vd;
            break;
            
        case '2':
            if(vy<=(SCR_YSIZE_TFT_320240-LCD_YSIZE_TFT_320240-vd)) vy += vd;
            break;
            
        case '\r':
            return;
            
        default:
            break;
        }
        //Uart_Printf("vx = %3d, vy = %3d\r\n", vx, vy);
        Lcd_MoveViewPort(vx, vy);
    }
}

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD单个象素的显示数据输出
 */
static void PutPixel(U32 x,U32 y,U16 c)
{
    if(x<SCR_XSIZE_TFT_320240 && y<SCR_YSIZE_TFT_320240)
        LCD_BUFER[(y)][(x)] = c;
}

/*-----------------------------------------------------------------------------
 *  320 x 240 TFT LCD全屏填充特定颜色单元或清屏
 */
void Lcd_ClearScr( U16 c)
{
    unsigned int x, y;
    
    for( y = 0 ; y < SCR_YSIZE_TFT_320240 ; y++ )
    {
        for( x = 0 ; x < SCR_XSIZE_TFT_320240 ; x++ )
        {
            LCD_BUFER[y][x] = c ;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  LCD屏幕显示垂直翻转
 *  LCD display is flipped vertically
 *  But, think the algorithm by mathematics point.
 *    3I2
 *    4 I 1
 *   --+--   <-8 octants  mathematical cordinate
 *    5 I 8
 *    6I7
 */
static void Glib_Line(int x1, int y1, int x2, int y2, U16 color)
{
    int dx, dy, e;
    
    dx = x2 - x1; 
    dy = y2 - y1;
    
    if(dx >= 0)
    {
        if(dy >= 0)        // dy>=0
        {
            if(dx >= dy)   // 1/8 octant
            {
                e = dy-dx/2;
                while(x1 <= x2)
                {
                    PutPixel(x1, y1, color);
                    if(e > 0) {y1+=1; e-=dx;}
                    x1 += 1;
                    e += dy;
                }
            }
            else        // 2/8 octant
            {
                e = dx-dy/2;
                while(y1 <= y2)
                {
                    PutPixel(x1, y1, color);
                    if(e > 0) {x1+=1; e-=dy;}
                    y1 += 1;
                    e += dx;
                }
            }
        }
        else           // dy<0
        {
            dy = -dy;   // dy=abs(dy)
            
            if(dx >= dy) // 8/8 octant
            {
                e = dy-dx/2;
                while(x1 <= x2)
                {
                    PutPixel(x1, y1, color);
                    if(e > 0) {y1-=1; e-=dx;}
                    x1 += 1;
                    e += dy;
                }
            }
            else        // 7/8 octant
            {
                e = dx-dy/2;
                while(y1 >= y2)
                {
                    PutPixel(x1, y1, color);
                    if(e > 0) {x1+=1; e-=dy;}
                    y1 -= 1;
                    e += dx;
                }
            }
        }
    }
    else //dx<0
    {
        dx=-dx;     //dx=abs(dx)
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
            else        // 3/8 octant
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
        else       // dy<0
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
            else        // 6/8 octant
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

/*-----------------------------------------------------------------------------
 *  在LCD屏幕上画一个矩形
 */
static void Glib_Rectangle(int x1, int y1, int x2, int y2, U16 color)
{
    Glib_Line(x1, y1, x2, y1, color);
    Glib_Line(x2, y1, x2, y2, color);
    Glib_Line(x1, y2, x2, y2, color);
    Glib_Line(x1, y1, x1, y2, color);
}

/*-----------------------------------------------------------------------------
 *  在LCD屏幕上用颜色填充一个矩形
 */
static void Glib_FilledRectangle(int x1, int y1, int x2, int y2, U16 color)
{
    int i;

    for(i = y1; i <= y2; i++) {    // 用n条直线填满区域!
        Glib_Line(x1, i, x2, i, color);
    }
}

/*-----------------------------------------------------------------------------
 *  在LCD屏幕上指定坐标点画一个指定大小的图片
 */
static void Paint_Bmp(int x0, int y0, int h, int l, unsigned char bmp[])
{
    int x, y;
    U32 c;
    int p = 0;
    
    for( y = 0 ; y < l ; y++ )
    {
        for( x = 0 ; x < h ; x++ )
        {
            c = bmp[p+1] | (bmp[p]<<8) ;
            
            if ( ( (x0+x) < SCR_XSIZE_TFT_320240) && ( (y0+y) < SCR_YSIZE_TFT_320240) )
                LCD_BUFER[y0+y][x0+x] = c ;
            
            p = p + 2 ;
        }
    }
}

/*-----------------------------------------------------------------------------
 *  test LTV350QV-F05
 */
void Test_Lcd_LTV350QVF05(void)
{
    int i;

//    Lcd_ClearScr(0xffff);       //fill all screen with some color
//    Glib_FilledRectangle(  50,  50,  270, 190, 0x0000);   //fill a Rectangle with some color
//    Glib_FilledRectangle( 100, 100,  220, 140, 0xf800);   //fill a Rectangle with some color
    
   #ifdef DEBUG
        Uart_Printf( "\r\nrGPBCON=0x%x\r\n", rGPBCON );
        Uart_Printf( "\trGPBUP=0x%x\r\n", rGPBUP );
        Uart_Printf( "rGPCCON=0x%x\r\n", rGPCCON );
        Uart_Printf( "\trGPCUP=0x%x\r\n", rGPCUP );
        Uart_Printf( "rGPDCON=0x%x\r\n", rGPDCON );
        Uart_Printf( "\trGPDUP=0x%x\r\n", rGPDUP );
        Uart_Printf( "rGPGCON=0x%x\r\n", rGPGCON );
        Uart_Printf( "\trGPGUP=0x%x\r\n\r\n", rGPGUP );
        
        Uart_Printf( "rLCDCON1=0x%x\r\n", rLCDCON1 );
        Uart_Printf( "rLCDCON2=0x%x\r\n", rLCDCON2 );
        Uart_Printf( "rLCDCON3=0x%x\r\n", rLCDCON3 );
        Uart_Printf( "rLCDCON4=0x%x\r\n", rLCDCON4 );
        Uart_Printf( "rLCDCON5=0x%x\r\n\r\n", rLCDCON5 );
        
        Uart_Printf( "rLCDSADDR1=0x%x\r\n", rLCDSADDR1 );
        Uart_Printf( "rLCDSADDR2=0x%x\r\n", rLCDSADDR2 );
        Uart_Printf( "rLCDSADDR3=0x%x\r\n\r\n", rLCDSADDR3 );
        
        Uart_Printf( "rLCDINTMSK=0x%x\r\n", rLCDINTMSK );
        Uart_Printf( "rLPCSEL=0x%x\r\n", rLPCSEL );
        Uart_Printf( "rTPAL=0x%x\r\n\r\n", rTPAL );
    #endif

    //LOGO

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
   
}

/* the lcd_240x320.c end */

