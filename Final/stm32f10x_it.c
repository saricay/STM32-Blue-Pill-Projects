/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and 
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"
#include <stdio.h>
/** @addtogroup STM32F10x_StdPeriph_Template
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M3 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	//delayDecrement();
}

void UART_Transmit(char *string)
{
	while(*string)
	{
		while(!(USART1->SR & 0x00000040));
		USART_SendData(USART1,*string);
		*string++;
	}
}
//Declared variables explained in the main.c file
extern char temperature[20];
extern float t;
extern int adc_value;
void read_sensor(void);
extern uint16_t dataBuffer[20];
extern float error_percent,overshoot_percent;
extern float reference_temperature;
///////////////////////////////////////////////////
//uint16_t temp1;
float read_temp; //Sensed temperature value came from the sensor

void TIM3_IRQHandler(void){
		if(TIM_GetITStatus(TIM3, TIM_IT_Update))
		{	
				t=t+0.001;
				if (t>0.25) //Transmit frequency is 4 Hz, Timer's is 1 kHz
				{
					UART_Transmit(temperature); 
					read_sensor();
					//temp1=(uint16_t) dataBuffer[0]<<8 | dataBuffer[1];
					//read_temp=(float)( temp1>>7)*0.5; //The temperature data which is combined from 2 different bytes
					read_temp=dataBuffer[0];
					sprintf(temperature,"%d\r",dataBuffer[0]);
					
					error_percent=reference_temperature-read_temp;
					error_percent=error_percent*100/reference_temperature;
					
					//Control algorithm of the system
					if (error_percent>=10){
						TIM3->CCR1 =0; //%0 duty cycle to give full power
						overshoot_percent=0;}
					
					else if (error_percent<10 && error_percent>=2){
						TIM3->CCR1 =(int)(1000/(reference_temperature-read_temp+1)); //Supplied Power is the function of the error
						overshoot_percent=0;
						GPIO_ResetBits(GPIOB, GPIO_Pin_15); 
						GPIO_ResetBits(GPIOB, GPIO_Pin_14); 
						GPIO_ResetBits(GPIOB, GPIO_Pin_13);}
					
					else if (error_percent<=2)
					{
						
							TIM3->CCR1 =1000; //%100 duty cycle to cut full power
						  overshoot_percent=-error_percent;
						
							//Overshoot indicator Leds
							if (error_percent>=-2 && error_percent<0 && adc_value>250){ //Because I didn't include 0 as <=0, the green Led didn't flash when the reference value is so close.
								GPIO_SetBits(GPIOB, GPIO_Pin_13); //Green Led
								GPIO_ResetBits(GPIOB, GPIO_Pin_14); 
								GPIO_ResetBits(GPIOB, GPIO_Pin_15); }
							
							else if(error_percent>-10 && error_percent<-2 && adc_value>250){
								GPIO_SetBits(GPIOB, GPIO_Pin_14); //Yellow Led
								GPIO_ResetBits(GPIOB, GPIO_Pin_15); 
								GPIO_ResetBits(GPIOB, GPIO_Pin_13); }
							
							else if(error_percent<-10 && adc_value>250){	
								GPIO_SetBits(GPIOB, GPIO_Pin_15); //Red Led
								GPIO_ResetBits(GPIOB, GPIO_Pin_14); 
								GPIO_ResetBits(GPIOB, GPIO_Pin_13); }
							
							else{
								GPIO_ResetBits(GPIOB, GPIO_Pin_15); 
								GPIO_ResetBits(GPIOB, GPIO_Pin_14); 
								GPIO_ResetBits(GPIOB, GPIO_Pin_13);
							}
								
					}
					
					t=0;
				} 
						
		}
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);

}


/******************************************************************************/
/*                 STM32F10x Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f10x_xx.s).                                            */
/******************************************************************************/

/**
  * @brief  This function handles PPP interrupt request.
  * @param  None
  * @retval None
  */
/*void PPP_IRQHandler(void)
{
}*/

/**
  * @}
  */ 


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
