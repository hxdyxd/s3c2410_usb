#ifndef	_UART_H
#define	_UART_H

void UartSetBaud(U32 ch, U32 baud);
char UartGetch(U32 ch);
char UartGetkey(U32 ch);
void UartPutch(U32 ch, U32 data);
void UartPuts(U32 ch, char *pt);
void UartPrintf(U32 ch, char *fmt,...);

#endif