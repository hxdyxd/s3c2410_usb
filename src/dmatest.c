#include "def.h"
#include "2410addr.h"
#include "2410lib.h"
#include "DMAAdmin.h"

#define	DMA_TEST	0x23
#define	DMA_ATTR	((DMA_TEST<<16)|SRC_LOC_AHB|SRC_ADDR_INC|DST_LOC_AHB|DST_ADDR_INC)
#define	DMA_MODE	(HANDSHAKE_MODE|SYNC_AHB|DONE_GEN_INT|TSZ_UNIT|WHOLE_SVC|SW_TRIG|RELOAD_OFF|DSZ_16b)

static volatile U8 dmaDone[4];

static void __irq Dma0Done(void)
{
    ClearPending(BIT_DMA0);
    dmaDone[0]=1;
    WrUTXH0('A');
}

static void __irq Dma1Done(void)
{
    ClearPending(BIT_DMA1);
    dmaDone[1]=1;
    WrUTXH0('B');
}

static void __irq Dma2Done(void)
{
    ClearPending(BIT_DMA2);
    dmaDone[2]=1;
    WrUTXH0('C');
}

static void __irq Dma3Done(void)
{
    ClearPending(BIT_DMA3);
    dmaDone[3]=1;
    WrUTXH0('D');
}

void DmaTest(void)
{
	int ch;	
//	U32 DmaAttr[4];
	U32 DmaTestReq[4];		
	
	puts("DMA test\r\n");		
	
	pISR_DMA0 = (U32)Dma0Done;	 
	ClearPending(BIT_DMA0);	
	EnableIrq(BIT_DMA0);
	
	pISR_DMA1 = (U32)Dma1Done;	 
	ClearPending(BIT_DMA1);	
	EnableIrq(BIT_DMA1);

	pISR_DMA2 = (U32)Dma2Done;	 
	ClearPending(BIT_DMA2);	
	EnableIrq(BIT_DMA2);

	pISR_DMA3 = (U32)Dma3Done;	 
	ClearPending(BIT_DMA3);	
	EnableIrq(BIT_DMA3);
	
	for(ch=0; ch<4; ch++)
		dmaDone[ch] = 1;
	for(ch=0; ch<4; ch++)
	{			
		DmaTestReq[ch] = RequestDMASW(DMA_ATTR, DMA_MODE);
//		printf("Request sw dma channel = %x\r\n", DmaTestReq[ch]);
		if(DmaTestReq[ch]==REQUEST_DMA_FAIL)
		{
			printf("request dma%x fail!\r\n", ch);
		}
		else
		{
			dmaDone[(DmaTestReq[ch]>>4)&0xf] = 0;			
			SetDMARun(DmaTestReq[ch]|DMA_START, 0x32000000+ch*SIZE_1M, 0x32800000+ch*SIZE_1M, SIZE_512K);			
		}
	}	
	
	
	
	while(!(dmaDone[0]&dmaDone[1]&dmaDone[2]&dmaDone[3]));
	
	DisableIrq(BIT_DMA0);
	DisableIrq(BIT_DMA1);
	DisableIrq(BIT_DMA2);
	DisableIrq(BIT_DMA3);
	for(ch=0; ch<4; ch++)	
		ReleaseDMA(DmaTestReq[ch]);	
	
	printf("\r\nDMA Transfer finished.\r\n");	
}