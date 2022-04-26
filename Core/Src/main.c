/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "dma.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ad9833.h"
#include "hmi.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

//ADC1 CH1 PA1
//ADC2 CH2 PA2
//ADC3 CH3 PA3
//usart1 PA9  PA10

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//extern DMA_HandleTypeDef hdma_adc1;
int flag_adc = 0;
struct adc_val {
	float maxx;
	float minn;
	float vpp;
	float ADCf[256];
	uint16_t ADCb[256];
} adc1, adc2, adc3off, adc3on;

float RL = 1963;
float gain1 = 0.0527, gain2 = 0.0579, gain3off = 5.1009, gain3on = 5.0885;

float Uin1, Uin2;
float Uo3AC, Uo3DC;
float Uo3ACRL;
unsigned int Rin, Rout, Gain;
int judgement=2, jud=8;
int divide;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void init_adc_val(uint8_t);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */



void get_vpp(int flag)
{
	init_adc_val(flag);
  switch (flag)
	{
		case 1:  //U1
		{
			HAL_ADC_Start_DMA(&hadc1,(uint32_t *)adc1.ADCb,256);
			while(!flag_adc)
			{
			}
			flag_adc = 0;
			for(uint16_t i=0;i<256;i++)
			{
				adc1.ADCf[i]=(float)(adc1.ADCb[i])/(float)(4096.0)*3;
				adc1.maxx = (adc1.maxx>adc1.ADCf[i]? adc1.maxx : adc1.ADCf[i]);
				adc1.minn = (adc1.minn<adc1.ADCf[i]? adc1.minn : adc1.ADCf[i]);
			}
			adc1.vpp=adc1.maxx-adc1.minn;
			break;
		}

		case 2: //U2
		{
			HAL_ADC_Start_DMA(&hadc2,(uint32_t *)adc2.ADCb,256);
						while(!flag_adc)
						{
						}
						flag_adc = 0;
			for(uint16_t i=0;i<256;i++)
			{
				adc2.ADCf[i]=(float)(adc2.ADCb[i])/(float)(4096.0)*3;
				adc2.maxx = (adc2.maxx>adc2.ADCf[i]? adc2.maxx : adc2.ADCf[i]);
				adc2.minn = (adc2.minn<adc2.ADCf[i]? adc2.minn : adc2.ADCf[i]);
			}
			adc2.vpp=adc2.maxx-adc2.minn;
			break;
		}

		case 3: //U3off
		{
			HAL_ADC_Start_DMA(&hadc3,(uint32_t *)adc3off.ADCb,256);
						while(!flag_adc)
						{
						}
						flag_adc = 0;
			for(uint16_t i=0;i<256;i++)
			{
				adc3off.ADCf[i]=(float)(adc3off.ADCb[i])/(float)(4096.0)*3;
				adc3off.maxx = (adc3off.maxx>adc3off.ADCf[i]? adc3off.maxx : adc3off.ADCf[i]);
				adc3off.minn = (adc3off.minn<adc3off.ADCf[i]? adc3off.minn : adc3off.ADCf[i]);
			}
			adc3off.vpp=adc3off.maxx-adc3off.minn;
			break;
		}

		case 4: //U3on
		{
			HAL_ADC_Start_DMA(&hadc3,(uint32_t *)adc3on.ADCb,256);
						while(!flag_adc)
						{
						}
						flag_adc = 0;
			for(uint16_t i=0;i<256;i++)
			{
				adc3on.ADCf[i]=(float)(adc3on.ADCb[i])/(float)(4096.0)*3;
				adc3on.maxx = (adc3on.maxx>adc3on.ADCf[i]? adc3on.maxx : adc3on.ADCf[i]);
				adc3on.minn = (adc3on.minn<adc3on.ADCf[i]? adc3on.minn : adc3on.ADCf[i]);
			}
			adc3on.vpp=adc3on.maxx-adc3on.minn;
			break;
		}
	}
	 return;
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
	flag_adc = 1;
}//传输完成中断

void init_adc_val(uint8_t flag)
{
	switch(flag)
	{
		case 1:
			adc1.maxx = 0;
			adc1.minn = 999;
			break;
		case 2:
			adc2.maxx = 0;
			adc2.minn = 999;
			break;
		case 3:
			adc3off.maxx = 0;
			adc3off.minn = 999;
			break;
		case 4:
			adc3on.maxx = 0;
			adc3on.minn = 999;
			break;
	}
	return;
}

void getTrueADCResult(void)
{
	Uin1 = gain1 * adc1.vpp;
	Uin2 = gain2 * adc2.vpp;
	Uo3AC = gain3off * adc3off.vpp;
	Uo3DC = gain3off * adc3off.maxx - Uo3AC / 2;
	Uo3ACRL = gain3on * adc3on.vpp;
}

void getRinAndRoutAndGainResult(void)
{
	Rin = Uin2 / (Uin1 - Uin2) * 7500;
	Rout = (Uo3AC - Uo3ACRL) / Uo3ACRL * RL;
	Gain = Uo3AC / Uin2;
}

void getDetectNeed(void)
{
	Uo3AC = gain3off * adc3off.vpp;
	Uin1 = gain1 * adc1.vpp;
	Uin2 = gain2 * adc2.vpp;
	Uo3DC = gain3off * adc3off.maxx - Uo3AC / 2;
	Rin = Uin2 / (Uin1 - Uin2) * 7500;
}

void getScanNeed()
{
	Uin2 = gain2 * adc2.vpp;
	Uo3AC = gain3off * adc3off.vpp;
	Gain = Uo3AC / Uin2;
	}

void averageDetet()
{
	unsigned int aveRin = 0;
	const int cir = 1;
	for(uint8_t i = 0; i < cir; i++)
	{
		HAL_Delay(1000);
		get_vpp(1);
		get_vpp(2);
		get_vpp(3);
		getDetectNeed();
		aveRin+=Rin;
	}
	Rin = aveRin/cir;
}


void checkError()
{
	AD9833_WaveSeting(1000,0,SIN_WAVE,0 );
	HMI_Send("page Detect");
	HAL_TIM_Base_Stop(&htim2);
	TIM2->ARR = 4000;
	HAL_TIM_Base_Start(&htim2);
	HAL_Delay(1000);

	RLoad_UnCarry;
	get_vpp(1);
	get_vpp(2);
	get_vpp(3);

	getDetectNeed();
	HAL_Delay(1000);

	if (Uo3DC > 8 || Uo3DC < 7)
	{
		if (Rin > 13000) HMI_Send("t0.txt=\"R1 open\"");
		else if (Rin > 7000) HMI_Send("t0.txt=\"R4 open\"");
		else if (Rin > 2000) HMI_Send("t0.txt=\"R3 short\"");
		else if(Uo3DC > 11)
		{
			if(Rin > 200)	HMI_Send("t0.txt=\"R1 short\"");
			else	HMI_Send("t0.txt=\"R2 short\"");
		}
		else if(Uo3DC > 4)	HMI_Send("t0.txt=\"R2 open\"");
		else if(Rin > 185)	HMI_Send("t0.txt=\"R3 open\"");
		else HMI_Send("t0.txt=\"R4 short\"");
	}
	else if (Rin < 2700 || Rin > 3000)
	{
		if (Rin < 0 || Rin > 50000) HMI_Send("t0.txt=\"C1 open\"");
		else if(Rin > 7000)	HMI_Send("t0.txt=\"C2 open\"");
		else HMI_Send("t0.txt=\"C2 double\"");
	}
	else
	{
		AD9833_WaveSeting(300000,0,SIN_WAVE,0 );
		HAL_TIM_Base_Stop(&htim2);
		TIM2->ARR = 93;//200000   68
		HAL_TIM_Base_Start(&htim2);
//		get_vpp(1);
//		get_vpp(2);
//		get_vpp(3);
//		getDetectNeed();
		averageDetet();
		if (Rin < 1000)
		{
			AD9833_WaveSeting(30,0,SIN_WAVE,0 );
			HAL_TIM_Base_Stop(&htim2);
			TIM2->ARR = 300000;//
			HAL_TIM_Base_Start(&htim2);
//			get_vpp(1);
//			get_vpp(2);
//			get_vpp(3);
//			getDetectNeed();
			averageDetet();
			if (Rin > 9350)  HMI_Send("t0.txt=\"C3 open\"");
			else HMI_Send("t0.txt=\"C1 double\"");
		}
		else
		{
			HMI_Send("t0.txt=\"C3 double\"");
		}
	}
	HAL_Delay(2000);
}

void getvalue()
{
	  HAL_TIM_Base_Stop(&htim2);
	  TIM2->ARR = 4000;
	  HAL_TIM_Base_Start(&htim2);
	  AD9833_WaveSeting(1000,0,SIN_WAVE,0 );

	  RLoad_UnCarry;
	  HAL_Delay(2000);
	  get_vpp(1);
	  get_vpp(2);
	  get_vpp(3);

	  RLoad_Carry;
	  HAL_Delay(2000);
	  get_vpp(4);
	  getTrueADCResult();

	  getRinAndRoutAndGainResult();

	  //HAL_Delay(2000);

	  int Conv;
	  HMI_Send("page Character");
	  HAL_Delay(1000);

	  Conv = ceil(adc1.vpp);
	  HMI_Send("t0.txt=\"Uin1=%d\"",Conv);
	  HAL_Delay(1000);
	  Conv = ceil(adc2.vpp);
	  HMI_Send("t1.txt=\"Uin2=%d\"",Conv);
	  HAL_Delay(1000);
	  Conv = ceil(adc3off.vpp);
	  HMI_Send("t2.txt=\"Uo3AC=%d\"",Conv);
	  HAL_Delay(1000);
	  Conv =ceil(adc3on.vpp);
	  HMI_Send("t3.txt=\"Uo3ACRL=%d\"",Conv);
	  HAL_Delay(1000);
	  Conv =ceil(Uo3DC);
	  HMI_Send("t4.txt=\"Uo3DC=%d\"",Conv);
	  HAL_Delay(4000);

	  HMI_Send("t0.txt=\"Rin=%d\"",Rin);
	  HAL_Delay(1000);
	  HMI_Send("t1.txt=\"Rout=%d\"",Rout);
	  HAL_Delay(1000);
	  HMI_Send("t2.txt=\"Gain=%d\"",Gain);
	  HAL_Delay(4000);
}

void ScanFreq()
{
	int Freq=30, Conv, Max=0, n=0, array[200], dB, Freq_HIGH=30, i;
	HMI_Send("page Curve");//
	HMI_Send("cle 1,0");
	RLoad_UnCarry;
	 while(Freq<291000)
	 {
		 AD9833_WaveSeting(Freq,0,SIN_WAVE,0 );
		 HAL_TIM_Base_Stop(&htim2);
		 if(Freq<1000)
		 {
			 divide = 84000000/(10*Freq);
			 Freq+=100;
		 }//10
		 else if(Freq<100000)
		 {
		   divide = 84000000/(10*Freq);
		   Freq+=3000;
		 }//33
		 else if(Freq<160000)
		 {
			 divide = 84000000/(7*Freq);//7.3
			 Freq+=1000;
		 }//60
		 else if(Freq<200000)
		 {
			 divide = 84000000/(5.3*Freq);//6.5
			 Freq+=2000;
		 }//30
		 else if(Freq<240000)
		  {
		 	divide = 84000000/(4.6*Freq);
		 	Freq+=3000;
		 }//27
		 else
		 {
			 divide = 84000000/(4*Freq);
			 Freq+=3000;
		 }
		 TIM2->ARR = divide;
		 HAL_TIM_Base_Start(&htim2);
		 get_vpp(2);
		 get_vpp(3);
		 getScanNeed();
		 Conv = Gain;
		 Max = (Max > Conv? Max : Conv);
		 HMI_Send("add 1,0,%d",Conv);
		 array[n] = Conv;
		 n++;
	 }

	 dB = 0.707*Max;
	 for(i=0;i<n;i++)
	 {
		 if(i>0 && Freq_HIGH<1000)
			 Freq_HIGH+=100;
		 else if(Freq_HIGH<100000)
			 Freq_HIGH+=3000;
		 else if(Freq_HIGH<160000)
		 	 Freq_HIGH+=1000;
		 else if(Freq_HIGH<220000)
		 	 Freq_HIGH+=2000;
		 else if(Freq_HIGH<300000)
		 	 Freq_HIGH+=3000;
		 if((array[i]-dB)<1 && (array[i]-dB)>-1)
		 {
			 HMI_Send("x0.val=%d",Freq_HIGH);
			 break;
		 }
	 }

	 HAL_Delay(4000);

}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART2)
	{
		if(jud == 17)
		{
			judgement=0;
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&jud, 1);//�?要重新接受上位机发来的数�?
		}
		else if(jud == 34)
		{
			judgement=1;
			HAL_UART_Receive_IT(&huart2, (uint8_t *)&jud, 1);//�?要重新接受上位机发来的数�?
		}
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_ADC1_Init();
  MX_ADC3_Init();
  MX_USART1_UART_Init();
  MX_ADC2_Init();
  MX_USART2_UART_Init();
  MX_TIM2_Init();
  /* USER CODE BEGIN 2 */
  HMI_Send("page Begin");
  HAL_UART_Receive_IT(&huart2, (uint8_t *)&jud, 1);
  HAL_TIM_Base_Start(&htim2);//以上为初始化

  //freq=1KHZ   ARR=4000
  //freq=200KHZ   ARR=83
  //freq=30HZ   ARR=420000-1


  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */




  while (1)
  {
	  //
	  if(judgement==0)
	  {
		  getvalue();
		  ScanFreq();
		  judgement=2;
		  HMI_Send("page Begin");
	  }
	  else if(judgement==1)
	  {
		  checkError();
		  judgement=2;
		  HMI_Send("page Begin");
	  }

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 168;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
