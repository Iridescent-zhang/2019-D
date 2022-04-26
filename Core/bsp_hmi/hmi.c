#include "hmi.h"

uint16_t hmi_flag;
uint8_t hmi_value;
extern uint16_t page_value;


void HMI_Send(char *format,...)
{
	va_list args;
	uint32_t length;
	uint8_t buffer[100];
	va_start(args,format);
	length = vsprintf((char *)buffer,format, args);
	va_end(args);
	HAL_UART_Transmit(&huart2,buffer,length,Uart_Timeout);
	length= sprintf((char *)buffer,"\xff\xff\xff");
	HAL_UART_Transmit(&huart2,buffer,length,Uart_Timeout);
}
