#ifndef __AD9833_H__
#define __AD9833_H__

#include "main.h"

#include "usart.h"
#include "gpio.h"


//                STM32                        AD9833


//                GPIOB_Pin_5                 ---> FSYNC


//                GPIOB_Pin_4                 ---> SCK

 
//                GPIOB_Pin_3                 ---> DAT

 
        #define FSYNC_0()              HAL_GPIO_WritePin(GPIOB, PIN_FSYNC_Pin, GPIO_PIN_RESET) 


        #define FSYNC_1()                 HAL_GPIO_WritePin(GPIOB, PIN_FSYNC_Pin, GPIO_PIN_SET)

 
        #define SCK_0()                HAL_GPIO_WritePin(GPIOB, PIN_SCLK_Pin, GPIO_PIN_RESET)
 
 
        #define SCK_1()                HAL_GPIO_WritePin(GPIOB, PIN_SCLK_Pin, GPIO_PIN_SET)


        #define DAT_0()               HAL_GPIO_WritePin(GPIOB, PIN_DAT_Pin, GPIO_PIN_RESET)

 
        #define DAT_1()                 HAL_GPIO_WritePin(GPIOB, PIN_DAT_Pin, GPIO_PIN_SET)


	#define TRI_WAVE                0


        #define SQU_WAVE                1

 
        #define SIN_WAVE               2  



void AD9833_Write(unsigned int TxData);
void AD9833_WaveSeting(float Freq,unsigned int Freq_SFR,unsigned int WaveMode,unsigned int Phase );

#endif
