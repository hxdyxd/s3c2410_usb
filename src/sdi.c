#include "def.h"
#include "option.h"
#include "2410addr.h"
#include "2410lib.h"
#include "sdi.h"

#define SD_DEBUG		0

#define INICLK	400000//300000
#define NORCLK	20000000

#define POL	0
#define INT	1
#define DMA	2

int CMD13(void);    // Send card status
int CMD9(void);
// Global variables
int *Tx_buffer;	//128[word]*16[blk]=8192[byte]
int *Rx_buffer;	//128[word]*16[blk]=8192[byte]
volatile unsigned int rd_cnt;
volatile unsigned int wt_cnt;
volatile unsigned int block;
volatile unsigned int TR_end=0;

int Wide=0; // 0:1bit, 1:4bit
int MMC=0;  // 0:SD  , 1:MMC

int  Maker_ID;
char Product_Name[7]; 
int  Serial_Num;

volatile int RCA;
static int blk_offset;

void SDI_test(void)
{
    RCA=0;
    MMC=0;

    rGPEUP  = 0xf83f;     // The pull up
    rGPECON = 0xaaaaaaaa;
    rSDICSTA = 0xffff;
    rSDIDSTA = 0xffff;

    printf("\r\nSDI CARD test\r\n");
    
    if( !SD_card_init() )
	return;
    
    TR_Buf_new();

	block = 1 ;    
	printf("Blocks = %d\r\n", block);  
	blk_offset = 0x2000;

    //CMD13();    
    
	Wt_Block();

    Rd_Block();

    View_Rx_buf();
    
#ifdef	TEST_MMC_STREAM
    if(MMC)
		TR_Buf_new();
    
    if(MMC)
    {
		Wt_Stream();

		Rd_Stream();

		View_Rx_buf();
    }
#endif
	Card_sel_desel(0);	// Card deselect
    
    	rSDICARG = RCA<<16;
		rSDICCON = (0x1<<10)|(0x1<<9)|(0x1<<8)|0x4a;
		if(Chk_CMDend(10, 1))
		{
			#if( SD_DEBUG )
				printf("CID = %x,%x,%x,%x\r\n", rSDIRSP0, rSDIRSP1, rSDIRSP2, rSDIRSP3);
			#endif
			Delay( 0 ) ;
		}
		else
			puts("Get cid fail!\r\n");

    if(!CMD9())
		printf("Get CSD fail!!!\r\n");
    rSDIDCON=0;//tark???
    rSDICSTA=0xffff;
}

void TR_Buf_new(void)
{
    //-- Tx & Rx Buffer initialize
    int i, j;
//    int start = 0x03020100;

    Tx_buffer=(int *)0x31000000;

    j=0;
    for(i=0;i<2048;i++)	//128[word]*16[blk]=8192[byte]
		*(Tx_buffer+i)=i;
//	*(Tx_buffer+i)=0x5555aaaa;
    Flush_Rx_buf();
/*
    for(i=0;i<20;i++){
        for(j=0;j<128;j++){
	Tx_buffer[j+i*128]=start;
	if(j % 64 == 63) start = 0x0302010;
	else start = start + 0x04040404;
        }
        start = 0x03020100;
    }
*/
}

void Flush_Rx_buf(void)
{
    //-- Flushing Rx buffer 
    int i;

    Rx_buffer=(int *)0x31800000;
    for(i=0;i<2048;i++)	//128[word]*16[blk]=8192[byte]
		*(Rx_buffer+i)=0;
//    printf("\r\n--End Rx buffer flush\r\n");
}

void View_Rx_buf()
{
    //-- Display Rx buffer 
    int i,error=0;
/*
    for(i=0;i<2048;i++)
	printf("RB[%02x]=%x,",Rx_buffer[i]);
*/
    Tx_buffer=(int *)0x31000000;
    Rx_buffer=(int *)0x31800000;
    printf("Check Rx data\r\n\r\n");
    printf("The follow is the data writed to SD Card just now:\r\n");

    for(i=0;i<128*block;i++)
    {
        if(Rx_buffer[i] != Tx_buffer[i])
		{
	    	printf("\r\nTx/Rx error\r\n"); 
			printf("%d:Tx-0x%08x, Rx-0x%08x\r\n",i,Tx_buffer[i], Rx_buffer[i]);
	    	error=1;
			break;
        }
        //printf(".");
        if( (i%15)==0 )		printf( "\r\n" ) ;
        printf("%02x,", Rx_buffer[i]);
    }
    if(!error)
		printf("\r\n\r\nSD CARD Write and Read test is OK!\r\n");
}

void View_Tx_buf(void)
{
    //-- Display Tx buffer 
    int i;

    for(i=0;i<2048;i++)
		printf("TB[%02x]=%x,",Tx_buffer[i]);
}


int SD_card_init(void)
{
//-- SD controller & card initialize 
    int i;
//    char key;


    /* Important notice for MMC test condition */
    /* Cmd & Data lines must be enabled pull up resister */

    rSDIPRE=PCLK/(2*INICLK)-1;	// 400KHz
    rSDICON=(1<<4)|(1<<1)|1;	// Type B, FIFO reset, clk enable
    rSDIBSIZE=0x200;		// 512byte(128word)
    rSDIDTIMER=0xffff;		// Set timeout count

    for(i=0;i<0x1000;i++);  // Wait 74SDCLK for MMC card

    //printf("rSDIRSP0=0x%x\r\n",rSDIRSP0);
    CMD0();
    printf("\r\nIn idle\r\n");

    //-- Check MMC card OCR
    if(Chk_MMC_OCR()) 
    {
		printf("\r\nIn MMC ready\r\n");
		MMC=1;
		goto RECMD2;
    }

    //printf("MMC check end!!\r\n");
    //-- Check SD card OCR
    	//DbgPause(0);    	
    if(Chk_SD_OCR()) 
        printf("\r\nIn SD ready\r\n");
    else
    {
		printf("\r\nInitialize fail\r\nNo Card assertion\r\n");
        return 0;
    }    	

RECMD2:
    //-- Check attaced cards, it makes card identification state
    rSDICARG=0x0;   // CMD2(stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x42; //lng_resp, wait_resp, start, CMD2

    //-- Check end of CMD2
    if(!Chk_CMDend(2, 1)) 
		goto RECMD2;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)


    printf("\r\nEnd id\r\n");

RECMD3:
    //--Send RCA
    rSDICARG=MMC<<16;	    // CMD3(MMC:Set RCA, SD:Ask RCA-->SBZ)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x43;	// sht_resp, wait_resp, start, CMD3

    //-- Check end of CMD3
    if(!Chk_CMDend(3, 1)) 
		goto RECMD3;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //--Publish RCA
    if(MMC) 
		RCA=1;
    else 
		RCA=( rSDIRSP0 & 0xffff0000 )>>16;

	#if( SD_DEBUG )
	    printf("RCA=0x%x\r\n",RCA);    	
	#endif

    //--State(stand-by) check
    if( rSDIRSP0 & 0x1e00!=0x600 )  // CURRENT_STATE check, modified by hzh???
		goto RECMD3;
		
	//	printf("rSDIRSP0=%x, %x, %x\r\n", rSDIRSP0, rSDIRSP0 & 0x1e00, rSDIRSP0 & 0x1e00!=0x600);
    
    printf("\r\nIn stand-by\r\n");    	
    
    rSDIPRE=PCLK/(2*NORCLK)-1;	// Normal clock=25MHz

    Card_sel_desel(1);	// Select

    if(!MMC)
		Set_4bit_bus();
    else
		Set_1bit_bus();							

    return 1;
}

void Card_sel_desel(char sel_desel)
{
    //-- Card select or deselect
    if(sel_desel)
    {
RECMDS7:	
	rSDICARG=RCA<<16;	// CMD7(RCA,stuff bit)
	rSDICCON= (0x1<<9)|(0x1<<8)|0x47;   // sht_resp, wait_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 1))
	    goto RECMDS7;
	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	//--State(transfer) check
	if( rSDIRSP0 & 0x1e00!=0x800 )	//???
	    goto RECMDS7;
    }
    else
    {
RECMDD7:	
	rSDICARG=0<<16;		//CMD7(RCA,stuff bit)
	rSDICCON=(0x1<<8)|0x47;	//no_resp, start, CMD7

	//-- Check end of CMD7
	if(!Chk_CMDend(7, 0))
	    goto RECMDD7;
	//rSDICSTA=0x800;	// Clear cmd_end(no rsp)
    }
}

void __irq Rd_Int(void)
{
    U32 i,status;

    status=rSDIFSTA;
    if( (status&0x200) == 0x200 )	// Check Last interrupt?
    {
	for(i=(status & 0x7f)/4;i>0;i--)	//read reamin
	{
	    *Rx_buffer++=rSDIDAT;
	    rd_cnt++;
	}
    }
    else if( (status&0x80) == 0x80 )	// Check Half interrupt?
    {
        for(i=0;i<8;i++)
        {
    	    *Rx_buffer++=rSDIDAT;
	    rd_cnt++;
	}
    }

    ClearPending(BIT_SDI);
}

void __irq Wt_Int(void)
{
    ClearPending(BIT_SDI);

    rSDIDAT=*Tx_buffer++;
    wt_cnt++;

    if(wt_cnt==128*block)
    {
	rINTMSK |= BIT_SDI;
	rSDIDAT=*Tx_buffer;
	TR_end=1;
    }
}

void __irq DMA_end(void)
{
    ClearPending(BIT_DMA0);
    rSDIDCON &= ~0x8000;	//clear DMA bit, added by hzh
    
    TR_end=1;
}

void Rd_Block(void)
{
    U32 mode;
    int status;

    rd_cnt=0;    
    printf("[Block read test]\r\n");

    mode = 0 ;
    printf("Mode : Polling read\r\n");


    rSDICON |= rSDICON|(1<<1);	// FIFO reset
//    mode=2;//tark
    if(mode!=2)
	rSDIDCON=(1<<19)|(1<<17)|(Wide<<16)|(2<<12)|(block<<0);
		// Rx after cmd, blk, 4bit bus, Rx start, blk num

    rSDICARG=blk_offset*0x200;	// CMD17/18(addr)	

RERDCMD:
    switch(mode)
    {
	case POL:
		puts("Poll read\r\n");			
	    if(block<2)	// SINGLE_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
		if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
		    goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		    goto RERDCMD;
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)	    

	    while(rd_cnt<128*block)	// 512*block bytes
	    {
		if((rSDIDSTA&0x20)==0x20) // Check timeout 
		{
		    rSDIDSTA=0x1<<0x5;  // Clear timeout flag
		    break;
		}
		status=rSDIFSTA;			
		if((status&0x1000)==0x1000)	// Is Rx data?
		{
		    *Rx_buffer++=rSDIDAT;
		    rd_cnt++;
		}
	    }
	    break;
	
	case INT:
		puts("Interrupt read\r\n");
	    pISR_SDI=(unsigned)Rd_Int;
	    rINTMSK = ~(BIT_SDI);
	    
	    rSDIIMSK=5;	// Last & Rx FIFO half int.

	    if(block<2)	// SINGLE_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
		if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
		    goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		    goto RERDCMD;
	    }
    
	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    while(rd_cnt<128*block);

	    rINTMSK |= (BIT_SDI);
	    rSDIIMSK=0;	// All mask
	    break;

	case DMA:
		puts("Dma read\r\n");				
	    pISR_DMA0=(unsigned)DMA_end;
	    rINTMSK = ~(BIT_DMA0);

	    rDISRC0=(int)(SDIDAT);	// SDIDAT
	    rDISRCC0=(1<<1)+(1<<0);	// APB, fix
	    rDIDST0=(U32)(Rx_buffer);	// Rx_buffer
	    rDIDSTC0=(0<<1)+(0<<0);	// AHB, inc
	    rDCON0=(1UL<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(2<<24)+(1<<23)+(1<<22)+(2<<20)+128*block;
	    //handshake, sync PCLK, TC int, single tx, single service, SDI, H/W request, 
	    //auto-reload off, word, 128blk*num
	    rDMASKTRIG0=(0<<2)+(1<<1)+0;    //no-stop, DMA2 channel on, no-sw trigger 

	    rSDIDCON=(1<<19)|(1<<17)|(Wide<<16)|(1<<15)|(2<<12)|(block<<0);
		    // Rx after rsp, blk, 4bit bus, dma enable, Rx start, blk num
	    if(block<2)	// SINGLE_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x51;    // sht_resp, wait_resp, dat, start, CMD17
		if(!Chk_CMDend(17, 1))	//-- Check end of CMD17
		    goto RERDCMD;	    
	    }
	    else	// MULTI_READ
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x52;    // sht_resp, wait_resp, dat, start, CMD18
		if(!Chk_CMDend(18, 1))	//-- Check end of CMD18 
		    return;//goto RERDCMD;
	    }
		
		#if( SD_DEBUG )
			printf("%x,%x,%x,%x,%x\r\n", rSDICCON, rSDICSTA, rSDIDCON, rSDIFSTA, block);
			printf("%x,%x,%x,%x,%x,%x,%x\r\n", rDISRCC0, rDISRC0, rDIDSTC0, rDIDST0, rDCON0, rDMASKTRIG0, rDSTAT0);	
		#endif
			
	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    while(!TR_end);
		//printf("rSDIFSTA=0x%x\r\n",rSDIFSTA);
	    rINTMSK |= (BIT_DMA0);
	    TR_end=0;
	    rDMASKTRIG0=(1<<2);	//DMA0 stop
	    break;

	default:
	    break;
    }
    //-- Check end of DATA
	if(mode==5) {
		int dsta, fsta, dcnt;
    	
		dsta = rSDIDSTA;
		fsta = rSDIFSTA;
		dcnt = rSDIDCNT;

		#if( SD_DEBUG )
	    	printf("SDIDSTA=%x,SDIFSTA=%x,SDIDCNT=%x\r\n", dsta, fsta, dcnt);
		#endif

		if((dcnt&0xffffff)!=0) {
			int i, d;			
			
			i =0 ;
			while(rSDIDSTA&1) {
				d = rSDIDAT;
				i++;
			}
		/*	while(rSDIFSTA&0x1000)
				d = rSDIDAT;
			while(rSDIDCNT&0xffffff)
				d = rSDIDAT;
			while(rSDIFSTA&0x1000)
				d = rSDIDAT;*/
		
		dsta = rSDIDSTA;
		fsta = rSDIFSTA;
		dcnt = rSDIDCNT;

		#if( SD_DEBUG )
			printf("SDIDSTA=%x,SDIFSTA=%x,SDIDCNT=%x,read cont=%d\r\n", dsta, fsta, dcnt, i);
		#endif
    	

    	}
	}
    
	printf("chk data end\r\n");    
	if(!Chk_DATend()) 
		printf("error\r\n");
	rSDIDSTA=0x10;	// Clear data Tx/Rx end    

    if(block>1)
    {
RERCMD12:    
	//--Stop cmd(CMD12)
	rSDICARG=0x0;	    //CMD12(stuff bit)
	rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;//sht_resp, wait_resp, start, CMD12

	//-- Check end of CMD12
	if(!Chk_CMDend(12, 1)) 
	    goto RERCMD12;
	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    }

}


void Rd_Stream(void)	// only for MMC, 3blk read
{
//    int i;
    int status, rd_cnt=0;

    if(MMC!=1)
    {
		printf("Stream read command supports only MMC!\r\n");
	return;
    }    
    printf("\r\n[Stream read test]\r\n");
    
RECMD11:
    rSDIDCON=(1<<19)|(0<<17)|(0<<16)|(2<<12);

    rSDICARG=0x0;   // CMD11(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4b;   //sht_resp, wait_resp, dat, start, CMD11

    while(rd_cnt<128*block)
    {
		if( (rSDIDSTA&0x20) == 0x20 )
		{
	    	printf("Rread timeout error");
		    return ;
		}
	    
		status=rSDIFSTA;
		if((status&0x1000)==0x1000)
		{
		    //*Rx_buffer++=rSDIDAT;
	    	//rd_cnt++;
		    Rx_buffer[rd_cnt++]=rSDIDAT;
		}
    }

    //-- Check end of CMD11
    if(!Chk_CMDend(11, 1)) 
		goto RECMD11;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //-- Check end of DATA
    rSDIDCON=(1<<19)|(0<<17)|(0<<16)|(1<<14)|(0<<12);
    while( rSDIDSTA&0x3 !=0x0 );
    if(rSDIDSTA!=0) 
		#if( SD_DEBUG )
			printf("rSDIDSTA=0x%x\r\n", rSDIDSTA);
		#endif
    rSDIDSTA=0xff;

STRCMD12:    
    //--Stop cmd(CMD12)
    rSDICARG=0x0;	    //CMD12(stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4c; //sht_resp, wait_resp, start, CMD12

    //-- Check end of CMD12
    if(!Chk_CMDend(12, 1)) 
		goto STRCMD12;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
/*
    //-- Display Rx data
    printf("\r\nRx data\r\n");
    for(i=0;i<128*2;i++)
    {
        if(Rx_buffer[i] != Tx_buffer[i])
	{
	    printf("%08x, %08x\r\n",Tx_buffer[i], Rx_buffer[i]);
	    break;
        }
    }
*/

    printf("\r\n--End stream read test\r\n");
}


void Wt_Block(void)
{
    U32 mode;
    int status;

    wt_cnt=0;    
    printf("[Block write test]\r\n");

    mode = 0 ;
	printf("Mode : Polling write\r\n" );

	
    rSDICON |= rSDICON|(1<<1);	// FIFO reset
//    mode=1;//tark
    if(mode!=2)
	rSDIDCON=(1<<20)|(1<<17)|(Wide<<16)|(3<<12)|(block<<0);
		    // Tx after rsp, blk, 4bit bus, Tx start, blk num

    rSDICARG=blk_offset*0x200;	    // CMD24/25(addr)

REWTCMD:
    switch(mode)
    {
	case POL:
	    if(block<2)	// SINGLE_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x58;	//sht_resp, wait_resp, dat, start, CMD24
		if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;
	    }
	    else	// MULTI_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x59;	//sht_resp, wait_resp, dat, start, CMD25
		if(!Chk_CMDend(25, 1))	//-- Check end of CMD25
		    goto REWTCMD;	    
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    
	    while(wt_cnt<128*block)
	    {
		status=rSDIFSTA;
		if((status&0x2000)==0x2000) 
		{
		    rSDIDAT=*Tx_buffer++;
		    wt_cnt++;
		}
	    }
	    break;
	
	case INT:
	    pISR_SDI=(unsigned)Wt_Int;
	    rINTMSK = ~(BIT_SDI);

	    rSDIIMSK=0x10;  // Tx FIFO half int.

	    if(block<2)	    // SINGLE_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x58;    //sht_resp, wait_resp, dat, start, CMD24
		if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;
	    }
	    else	    // MULTI_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x59;    //sht_resp, wait_resp, dat, start, CMD25
		if(!Chk_CMDend(25, 1))	//-- Check end of CMD25 
		    goto REWTCMD;
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    while(!TR_end);
	    //while(wt_cnt<128);

	    rINTMSK |= (BIT_SDI);
	    TR_end=0;
	    rSDIIMSK=0;	// All mask
	    break;

	case DMA:
	    pISR_DMA0=(unsigned)DMA_end;
	    rINTMSK = ~(BIT_DMA0);

	    rDISRC0=(int)(Tx_buffer);	// Tx_buffer
	    rDISRCC0=(0<<1)+(0<<0);	// AHB, inc
	    rDIDST0=(U32)(SDIDAT);	// SDIDAT
	    rDIDSTC0=(1<<1)+(1<<0);	// APB, fix
	    rDCON0=(1UL<<31)+(0<<30)+(1<<29)+(0<<28)+(0<<27)+(2<<24)+(1<<23)+(1<<22)+(2<<20)+128*block;
	    //handshake, sync PCLK, TC int, single tx, single service, SDI, H/W request, 
	    //auto-reload off, word, 128blk*num
	    rDMASKTRIG0=(0<<2)+(1<<1)+0;    //no-stop, DMA0 channel on, no-sw trigger
	    
	    rSDIDCON=(1<<20)|(1<<17)|(Wide<<16)|(1<<15)|(3<<12)|(block<<0);
		    // Tx after rsp, blk, 4bit bus, dma enable, Tx start, blk num
	    if(block<2)	    // SINGLE_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x58;    //sht_resp, wait_resp, dat, start, CMD24
		if(!Chk_CMDend(24, 1))	//-- Check end of CMD24
		    goto REWTCMD;	    
	    }
	    else	    // MULTI_WRITE
	    {
		rSDICCON=(0x1<<9)|(0x1<<8)|0x59;    //sht_resp, wait_resp, dat, start, CMD25
		if(!Chk_CMDend(25, 1))	//-- Check end of CMD25 
		    goto REWTCMD;	    
	    }

	    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	    while(!TR_end);		

	    rINTMSK |= (BIT_DMA0);
	    TR_end=0;
	    rDMASKTRIG0=(1<<2);	//DMA0 stop

	    break;

	default:
	    break;
    }
    
    //-- Check end of DATA
    if(!Chk_DATend()) 
	printf("dat error\r\n");

    rSDIDSTA=0x10;	// Clear data Tx/Rx end

    if(block>1)
    {
	//--Stop cmd(CMD12)
REWCMD12:    
	rSDIDCON=(1<<18)|(1<<17)|(0<<16)|(1<<12)|(block<<0);
	
	rSDICARG=0x0;	    //CMD12(stuff bit)
	rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;    //sht_resp, wait_resp, start, CMD12

	//-- Check end of CMD12
	if(!Chk_CMDend(12, 1)) 
	    goto REWCMD12;
	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

	//-- Check end of DATA(with busy state)
	if(!Chk_BUSYend()) 
	    printf("error\r\n");
	rSDIDSTA=0x08;
    }
}

void Wt_Stream(void)	// only for MMC, 3blk write
{
    int status, wt_cnt=0;

    if(MMC!=1)
    {
	printf("Stream write command supports only MMC!\r\n");
	return;
    }
    printf("\r\n[Stream write test]\r\n");
RECMD20:
    rSDIDCON=(1<<20)|(0<<17)|(0<<16)|(3<<12);  // stream mode

    rSDICARG=0x0;	// CMD20(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x54;    //sht_resp, wait_resp, dat, start, CMD20

    //-- Check end of CMD25
    if(!Chk_CMDend(20, 1)) 
	goto RECMD20;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    while(wt_cnt<128*block)
    {
	status=rSDIFSTA;
	if((status&0x2000)==0x2000) 
	    rSDIDAT=Tx_buffer[wt_cnt++];
    }

    //-- Check end of DATA
    while( rSDIFSTA&0x400 );
    Delay(10);    // for the empty of DATA line(Hardware)
    rSDIDCON=(1<<20)|(0<<17)|(0<<16)|(1<<14)|(0<<12);
    while( (rSDIDSTA&0x3)!=0x0 );
    if(rSDIDSTA!=0) 
	printf("rSDIDSTA=0x%x\r\n", rSDIDSTA);
    rSDIDSTA=0xff;

STWCMD12:    
    //--Stop cmd(CMD12)
    rSDIDCON=(1<<18)|(1<<17)|(0<<16)|(1<<12);

    rSDICARG=0x0;	    //CMD12(stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4c;   //sht_resp, wait_resp, start, CMD12

    //-- Check end of CMD12
    if(!Chk_CMDend(12, 1)) 
	goto STWCMD12;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)

    //-- Check end of DATA(with busy state)
    if(!Chk_BUSYend()) 
	printf("error\r\n");
    rSDIDSTA=0x08;

    printf("\r\n--End Stream write test\r\n");
}


int Chk_CMDend(int cmd, int be_resp)
//0: Timeout
{
    int finish0;

    if(!be_resp)    // No response
    {
    	finish0=rSDICSTA;
		while((finish0&0x800)!=0x800)	// Check cmd end
	    finish0=rSDICSTA;

		rSDICSTA=finish0;// Clear cmd end state

		#if( SD_DEBUG )
			printf("%x\r\n", finish0);
		#endif

		return 1;
    }
    else	// With response
    {
    	finish0=rSDICSTA;
		while( !( ((finish0&0x200)==0x200) | ((finish0&0x400)==0x400) ))    // Check cmd/rsp end
    	    finish0=rSDICSTA;

		#if( SD_DEBUG )
			printf("CMD%d:rSDICSTA=0x%x, rSDIRSP0=0x%x\r\n",cmd, rSDICSTA, rSDIRSP0);   
		#endif
		 	    

		if(cmd==1 | cmd==9 | cmd==41)	// CRC no check
		{
		    if( (finish0&0xf00) != 0xa00 )  // Check error
	    	{
				rSDICSTA=finish0;   // Clear error state					

				if(((finish0&0x400)==0x400))
				{
					#if( SD_DEBUG )
						printf("CMD%d Time out!\r\n", cmd);
					#endif
					
			    	return 0;	// Timeout error		    	
		    	}
		    	
			}				
		    rSDICSTA=finish0;	// Clear cmd & rsp end state
		    //	printf("%x\r\n", finish0);
		}
		else	// CRC check
		{
	    	if( (finish0&0x1f00) != 0xa00 )	// Check error
		    {				
				rSDICSTA=finish0;   // Clear error state

				if(((finish0&0x400)==0x400))
				{
					#if( SD_DEBUG )	
						printf("CMD%d Time out!\r\n", cmd);
					#endif
					
			    	return 0;	// Timeout error
				}
			}
    		rSDICSTA=finish0;
		}
		return 1;
    }
}

int Chk_DATend(void)
{
    int finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x10)==0x10) | ((finish&0x20)==0x20) )) {
	// Chek timeout or data end
	finish=rSDIDSTA;
//		printf("%x\r\n", finish);
	}
    if( (finish&0xfc) != 0x10 )
    {
        printf("DATA:finish=0x%x\r\n", finish);
        rSDIDSTA=0xec;  // Clear error state
        return 0;
    }
    return 1;
}

int Chk_BUSYend(void)
{
    int finish;

    finish=rSDIDSTA;
    while( !( ((finish&0x08)==0x08) | ((finish&0x20)==0x20) ))
	finish=rSDIDSTA;

    if( (finish&0xfc) != 0x08 )
    {
        printf("DATA:finish=0x%x\r\n", finish);
        rSDIDSTA=0xf4;  //clear error state
        return 0;
    }
    return 1;
}

void CMD0(void)
{
    //-- Make card idle state 
    rSDICARG=0x0;	    // CMD0(stuff bit)
    rSDICCON=(1<<8)|0x40;   // No_resp, start, CMD0

    //-- Check end of CMD0
    Chk_CMDend(0, 0);
    //rSDICSTA=0x800;	    // Clear cmd_end(no rsp)
}

int Chk_MMC_OCR(void)
{
    int i;

    //-- Negotiate operating condition for MMC, it makes card ready state
    for(i=0;i<10;i++)
    {
	//	rSDICARG=0xffc000;	    	    //CMD1(OCR:2.6V~3.6V)
		rSDICARG=0xff8000;	    	    //CMD1(OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x41;    //sht_resp, wait_resp, start, CMD1

    	//-- Check end of CMD1
	//	if(Chk_CMDend(1, 1) & rSDIRSP0==0x80ffc000) 
		if(Chk_CMDend(1, 1) & rSDIRSP0==0x80ff8000) 
		{
	    	//rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
		    return 1;	// Success
		}
    }
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}

int Chk_SD_OCR(void)
{
    int i;

    //-- Negotiate operating condition for SD, it makes card ready state
    for(i=0;i<10;i++)
    {
    	CMD55();    // Make ACMD    	

    	rSDICARG=0xff8000;	//ACMD41(OCR:2.7V~3.6V)
    	rSDICCON=(0x1<<9)|(0x1<<8)|0x69;//sht_resp, wait_resp, start, ACMD41

	//-- Check end of ACMD41
    	if( Chk_CMDend(41, 1) & rSDIRSP0==0x80ff8000 ) //OCR最高位为0表示正在POWER UP
		{
		    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
	    	return 1;	// Success	    
		}			
		Delay(200); // Wait Card power up status
    }
    //printf("SDIRSP0=0x%x\r\n",rSDIRSP0);
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 0;		// Fail
}

int CMD55(void)
{
    //--Make ACMD
    rSDICARG=RCA<<16;			//CMD7(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x77;	//sht_resp, wait_resp, start, CMD55

    //-- Check end of CMD55
    if(!Chk_CMDend(55, 1))    
		return 0;	

    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

int CMD13(void)//SEND_STATUS
{
    int response0;

    rSDICARG=RCA<<16;			// CMD13(RCA,stuff bit)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x4d;	// sht_resp, wait_resp, start, CMD13

    //-- Check end of CMD13
    if(!Chk_CMDend(13, 1)) 
	return 0;
    //printf("rSDIRSP0=0x%x\r\n", rSDIRSP0);
    if(rSDIRSP0&0x100)
	printf("Ready for Data\r\n");
    else 
	printf("Not Ready\r\n");
    response0=rSDIRSP0;
    response0 &= 0x3c00;
    response0 = response0 >> 9;
    printf("Current Status=%d\r\n", response0);
    if(response0==6)
	SDI_test();

    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
    return 1;
}

int CMD9(void)//SEND_CSD
{
    rSDICARG=RCA<<16;				// CMD9(RCA,stuff bit)
    rSDICCON=(0x1<<10)|(0x1<<9)|(0x1<<8)|0x49;	// long_resp, wait_resp, start, CMD9

	#if( SD_DEBUG ) 
    	printf("\r\n****CSD register****\r\n");
	#endif

    //-- Check end of CMD9
    if(!Chk_CMDend(9, 1)) 
		return 0;

	#if( SD_DEBUG ) 
    	printf(" SDIRSP0=0x%x\r\n SDIRSP1=0x%x\r\n SDIRSP2=0x%x\r\n SDIRSP3=0x%x\r\n", rSDIRSP0,rSDIRSP1,rSDIRSP2,rSDIRSP3);
   	#endif
   	
   	return 1;
}

void Set_1bit_bus(void)
{
    Wide=0;
    if(!MMC)
	SetBus();
    printf("\r\n****1bit bus****\r\n");
}

void Set_4bit_bus(void)
{
    Wide=1;
    SetBus();
    printf("\r\n****4bit bus****\r\n");
}

void SetBus(void)
{
SET_BUS:
    CMD55();	// Make ACMD
    //-- CMD6 implement
    rSDICARG=Wide<<1;	    //Wide 0: 1bit, 1: 4bit
    rSDICCON=(0x1<<9)|(0x1<<8)|0x46;	//sht_resp, wait_resp, start, CMD55

    if(!Chk_CMDend(6, 1))   // ACMD6
	goto SET_BUS;
    //rSDICSTA=0xa00;	    // Clear cmd_end(with rsp)
}

void Set_Prt(void)
{
    //-- Set protection addr.0 ~ 262144(32*16*512) 
    printf("[Set protection(addr.0 ~ 262144) test]\r\n");

RECMD28:
    //--Make ACMD
    rSDICARG=0;	    // CMD28(addr) 
    rSDICCON=(0x1<<9)|(0x1<<8)|0x5c;	//sht_resp, wait_resp, start, CMD28

    //-- Check end of CMD28
    if(!Chk_CMDend(28, 1)) 
	goto RECMD28;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
}

void Clr_Prt(void)
{
    //-- Clear protection addr.0 ~ 262144(32*16*512) 
    printf("[Clear protection(addr.0 ~ 262144) test]\r\n");

RECMD29:
    //--Make ACMD
    rSDICARG=0;	    // CMD29(addr)
    rSDICCON=(0x1<<9)|(0x1<<8)|0x5d;	//sht_resp, wait_resp, start, CMD29

    //-- Check end of CMD29
    if(!Chk_CMDend(29, 1)) 
	goto RECMD29;
    //rSDICSTA=0xa00;	// Clear cmd_end(with rsp)
}

