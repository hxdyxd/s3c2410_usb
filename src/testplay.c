#include "2410addr.h"
#include "AudioDrv.h"
#include "2410lib.h"
#include "DmaAdmin.h"
#include "WindowsXP_Wav.h"		//长度为243552个字节

#define	BUF_SIZE	(16*1024)

extern U32 downloadAddress;
extern U32 downloadFileSize;
extern U8 USB_OR_UART_Download_OK ;

//static U16 fsTable[] = {8000,11025,16000,22050,32000,44100,48000,0};

void PlayMusicTest(void)
{
	int size, i, j, err;
	WAVEFORMATEX fmt;
	WAVEHDR hdr[2048];
	HWAVEOUT hwo;	
	U8 pause = 0;
	U8 mute = 0;	
	U32 volume;
	unsigned char *buf;
	
	if( USB_OR_UART_Download_OK == 0 )
	{
		downloadAddress = _NONCACHE_STARTADDRESS;
		buf = (unsigned char *)downloadAddress ;
		for( i = 0; i < 243552; i++ )  buf[i] = WindowsXP_Wav[i] ;
		downloadFileSize = 243552 ;
	}
	
	size = *(U32 *)(downloadAddress+0x28);
	i = 0;							
	
	while(size>0)
	{
		hdr[i].lpData = (LPSTR)(downloadAddress+0x2c+i*BUF_SIZE);
		hdr[i].dwBufferLength = (size>BUF_SIZE)?BUF_SIZE:size;	
		size -= BUF_SIZE;
		i++;
	}
	
	fmt.wFormatTag		= WAVE_FORMAT_PCM;
	fmt.nChannels		= *(U16 *)(downloadAddress+0x16);
	fmt.nSamplesPerSec	= *(U32 *)(downloadAddress+0x18);
	fmt.nAvgBytesPerSec	= *(U32 *)(downloadAddress+0x1c);
	fmt.nBlockAlign		= *(U16 *)(downloadAddress+0x20);
	fmt.wBitsPerSample	= *(U16 *)(downloadAddress+0x22);
	printf("\r\nSample Rate = %d, Channels = %d, %dBitsPerSample, size = %d\r\n",
			fmt.nSamplesPerSec, fmt.nChannels, fmt.wBitsPerSample, *(U32 *)(downloadAddress+0x28));
	
	hwo = 0;
	err = waveOutOpen(&hwo,
				0,
				&fmt,
				0,
				0,
				0);
	printf("\r\nerr = %x\r\n", err);			
	for(j=0;j<i;j++)
		waveOutWrite(0,	&hdr[j], 0);
		
	puts("Now playing the file\r\n");
	puts("Press 'ESC' to quit, '+' to inc volume, '-' to dec volume, 'm' to mute, 'p' to pause\r\n");		
	
	waveOutGetVolume(0,	&volume);
	while(1)
	{
		U8 key = getch();
		if( key == ESC_KEY )
			break;
		if(key=='p')
		{
			pause ^= 1;
			if(pause&1)
				waveOutPause(0);
			else
				waveOutRestart(0);				
		}
		if(key=='m')
		{						
			mute ^= 1;
			if(mute&1)									
				waveOutSetVolume(0, 0);			
			else
				waveOutSetVolume(0, volume);			
		}
		if((key=='+')&&(volume<=64535))		
		{
			volume += 1000;
			waveOutSetVolume(0, volume);
		}
		if((key=='-')&&(volume>=1000))
		{
			volume -= 1000;
			waveOutSetVolume(0, volume);
		}	
					
	}					
	waveOutClose(0);
}

void RecordTest(void)
{
	int size, i, j, err;
	WAVEFORMATEX fmt;
	WAVEHDR hdr[2048];
	HWAVEIN hwi = 1;	
	
	/*puts("Please select Sample Rate:\r\n");
	for(i=0; fsTable[i]; i++)
		printf("%d. %dHz\r\n", i, fsTable[i]);	
	while(1)
	{
		U8 key = getch();
		if((key-'0')>=0&&(key-'0'<=i-1))
		{
			fmt.nSamplesPerSec = fsTable[key-'0'];
			break;
		}		
	}*/
	
	printf( "\r\nThe Frequency of record is 48KHz\r\n" );
	fmt.nSamplesPerSec = 48000;		//采样频率为48KHz
	fmt.wBitsPerSample = 16;

	fmt.wFormatTag		= WAVE_FORMAT_PCM;
	fmt.nChannels		= 2;
	fmt.nBlockAlign     = fmt.wBitsPerSample*fmt.nChannels/8;	
	fmt.nAvgBytesPerSec	= fmt.nSamplesPerSec*fmt.nBlockAlign;
	
	downloadAddress  = 0x30800000;//_NONCACHE_STARTADDRESS;
	downloadFileSize = size = 16*1024*1024;	
	i = 0;
	
	while(size>0)
	{
		hdr[i].lpData = (LPSTR)(downloadAddress+0x2c+i*BUF_SIZE);
		hdr[i].dwBufferLength = (size>BUF_SIZE)?BUF_SIZE:size;	
		size -= BUF_SIZE;
		i++;
	}	
	
	*(U16 *)(downloadAddress+0x14) = fmt.wFormatTag;
	*(U16 *)(downloadAddress+0x16) = fmt.nChannels;
	*(U32 *)(downloadAddress+0x18) = fmt.nSamplesPerSec;
	*(U32 *)(downloadAddress+0x1c) = fmt.nAvgBytesPerSec;
	*(U16 *)(downloadAddress+0x20) = fmt.nBlockAlign;
	*(U16 *)(downloadAddress+0x22) = fmt.wBitsPerSample;
	*(U32 *)(downloadAddress+0x28) = downloadFileSize;
	
	err = waveInOpen(&hwi,
				0,
				&fmt,
				0,
				0,
				0);
	printf("\r\nerr = %x\r\n", err);
	
	for(j=0;j<i;j++)
		if(waveInAddBuffer(hwi, &hdr[j], 0))
			puts("Add buffer error!");
			
	printf("Added %d buffer for record\r\n", i);
	puts("Press any to Record\r\n");
	getch();
	
	puts("Now begin recording, Press 'ESC' to quit\r\n");	
	waveInStart(hwi);
	
	while(1)
	{
		U8 key;
		
		key = getkey();
		if( key == ESC_KEY )
			break;
			
//		printf("%x,%x,%x,%x,%x,%x,%x\r\n", rDISRC2, rDISRCC2, rDIDST2, rDIDSTC2, rDCON2, rDSTAT2, rDMASKTRIG2);		
//		printf("%x,%x,%x,%x\r\n", rIISCON, rIISMOD, rIISPSR, rIISFCON);
	}		
	waveInClose(hwi);
}

void RecordChannelOnOff(void)
{
	static int ChannelOn = 0;
	int err;
	WAVEFORMATEX fmt;
	WAVEHDR hdr;
	HWAVEIN hwi = 1;
	
	fmt.nSamplesPerSec  = 22050;
	fmt.wBitsPerSample  = 16;
	fmt.wFormatTag		= WAVE_FORMAT_PCM;
	fmt.nChannels		= 2;
	fmt.nBlockAlign     = fmt.wBitsPerSample*fmt.nChannels/8;	
	fmt.nAvgBytesPerSec	= fmt.nSamplesPerSec*fmt.nBlockAlign;
	
	hdr.lpData = (LPSTR)0x30800000;//_NONCACHE_STARTADDRESS;
	hdr.dwBufferLength = BUF_SIZE;
	
	if(!ChannelOn) {
		err = waveInOpen(&hwi,
					0,
					&fmt,
					0,
					0,
					0);
		printf("\r\nerr = %x\r\n", err);
		if(!err) {
			waveInAddBuffer(hwi, &hdr, 0);
			waveInStart(hwi);
			puts("Record channel on\r\n");
			ChannelOn = 1;
		}
	} else {
		waveInClose(hwi);
		puts("Record channel off\r\n");
		ChannelOn = 0;
	}
}
