/**************************************************************
Touch Screen control
**************************************************************/
#include <string.h>
#include "2410addr.h"
#include "2410lib.h"
#include "Touch_Screen.h"

#define ADCPRS 39

#define ITERATION 5	
unsigned int buf[ITERATION][2];
//*************************************************************

/**************************************************************
The interrupt function for Touch Screen
**************************************************************/
void __irq Touch_Screen(void)
{
    int i;
    
    rINTSUBMSK |= (BIT_SUB_ADC | BIT_SUB_TC);     //Mask sub interrupt (ADC and TC) 

    Uart_Printf("\r\nTS Down!\r\n");
        
      //Auto X-Position and Y-Position Read
    rADCTSC=(1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);
          //[7] YM=GND, [6] YP is connected with AIN[5], [5] XM=Hi-Z, [4] XP is connected with AIN[7]
          //[3] XP pull-up disable, [2] Auto(sequential) X/Y position conversion mode, [1:0] No operation mode    

    for(i=0;i<ITERATION;i++)
    {
        rADCTSC  = (1<<7)|(1<<6)|(0<<5)|(1<<4)|(1<<3)|(1<<2)|(0);            
        rADCCON |= 0x1;             //Start Auto conversion
        while(rADCCON & 0x1);       //Check if Enable_start is low
        while(!(0x8000&rADCCON));   //Check ECFLG
    
        buf[i][0] = (0x3ff&rADCDAT0);
        buf[i][1] = (0x3ff&rADCDAT1);
    }

    for(i=0;i<ITERATION;i++)    
        Uart_Printf("X %4d, Y %4d\r\n", buf[i][0], buf[i][1]);
         
    rADCTSC = (1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);  
      //[7] YM=GND, [6] YP is connected with AIN[5], [5] XM=Hi-Z, [4] XP is connected with AIN[7]
      //[3] XP pull-up enable, [2] Normal ADC conversion, [1:0] Waiting for interrupt mode                
    rSUBSRCPND |= BIT_SUB_TC;
    rINTSUBMSK  =~(BIT_SUB_TC);   //Unmask sub interrupt (TC)     
    ClearPending(BIT_ADC);
}
            
/**************************************************************
The Initial of Touch Screen
**************************************************************/
void Touch_Screen_Init(void)
{
    rADCDLY = (30000);    // ADC Start or Interval Delay

    rADCCON = (1<<14)|(ADCPRS<<6)|(0<<3)|(0<<2)|(0<<1)|(0); 
      // Enable Prescaler,Prescaler,AIN5/7 fix,Normal,Disable read start,No operation
    rADCTSC = (0<<8)|(1<<7)|(1<<6)|(0<<5)|(1<<4)|(0<<3)|(0<<2)|(3);//tark
      // Down,YM:GND,YP:AIN5,XM:Hi-z,XP:AIN7,XP pullup En,Normal,Waiting for interrupt mode

	rSUBSRCPND |= BIT_SUB_TC;
    rINTSUBMSK  =~(BIT_SUB_TC);
    
    pISR_ADC    = (unsigned)Touch_Screen;
    rINTMSK    &= ~(BIT_ADC);
    rINTSUBMSK &= ~(BIT_SUB_TC);
    Uart_Printf( "\r\nNow touchpanel controler is initial!\r\n" ) ;
    Uart_Printf( "Please Press it with touch pen and see what happend\r\n" ) ;
    Uart_Printf( "\r\nAny key to exit the touchpanel test\r\n" ) ;
    Uart_Getch() ;
    
    Touch_Screen_Off() ;
}
//*************************************************************
void Touch_Screen_Off(void)
{
	rINTMSK    |= (BIT_ADC);
    rINTSUBMSK |= (BIT_SUB_TC);
}
