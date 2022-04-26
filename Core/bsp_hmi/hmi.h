#ifndef _hmi_h
#define _hmi_h


#include "stdio.h"
#include "stdarg.h"
#include "usart.h"

#define Uart_Timeout 0xFFFF

void HMI_Send(char *format,...);

#endif
