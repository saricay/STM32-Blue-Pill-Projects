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


extern float t;
extern int button_situation;

extern double K,wn,zeta;
extern double u;

extern volatile double stepSize;
extern volatile double inputToSystem;
extern volatile double outputToPC;
extern double control_function_output;

extern double y1;
extern double y2;
extern double y3;
/*
//Second order system declerations
double ExplicitEulerEq1(double (*f)(double,double,double), double y2, double ts, double uk);
double ExplicitEulerEq2(double (*f)(double,double,double), double y1, double ts, double uk);
double eq1(double y1, double y2, double u);
double eq2(double y2,double y1, double u);
*/
/*
//Third order system declerations
double eq1(double y1,double y2, double u);
double eq2(double y2,double y3, double u);
double eq3(double y1,double y2,double y3, double u);
double ExplicitEulerEq1(double (*f)(double,double,double), double y2, double ts, double uk);
double ExplicitEulerEq2(double (*f)(double,double,double), double y3, double ts, double uk);
double ExplicitEulerEq3(double (*f)(double,double,double,double), double y1, double y2, double ts, double uk);
*/
extern double real_system_voltage;
extern double hardware_in_loop_value;
double PID(double r, double y);

//////////////////////////////////////////////////////////////////
void UART_Transmit(char *string)
{
	while(*string)
	{
		while(!(USART1->SR & 0x00000040));
		USART_SendData(USART1,*string);
		*string++;
	}
}
/////////////////////////////////////////////////////////////////
extern char output[20];
void TIM3_IRQHandler(void) {
		if (TIM_GetITStatus(TIM3, TIM_IT_Update) == SET){
			t=t+0.01;
			//control_function_output=PID(u,real_system_voltage)*button_situation; //P & PI Control value[in voltage]
			control_function_output=PID(u,outputToPC)*button_situation; //P & PI Control value[in voltage]
			TIM3->CCR2= (u/3.3)*400; //Open loop control reference value[in bits]
			//TIM3->CCR2= (control_function_output/3.3)*400; //CCR2 is used to create control value[in bits]
			
			if(t>=0.2){
				UART_Transmit(output);
				t=0;}
			/*
			 //2nd Order System Step Response
			y1 = ExplicitEulerEq1(eq1, y2, stepSize, inputToSystem);
			y2 = ExplicitEulerEq2(eq2, y1, stepSize, inputToSystem);
			//outputToPC = y1;
			hardware_in_loop_value=y1;
			*/
			/*
			//3rd Order System Step Response
			y1 = ExplicitEulerEq1(eq1, y2, stepSize, inputToSystem);
			y2 = ExplicitEulerEq2(eq2, y3, stepSize, inputToSystem);
			y3 = ExplicitEulerEq3(eq3, y1, y2, stepSize, inputToSystem);
			//outputToPC = y1;
			hardware_in_loop_value=y1;
			*/
		}
		
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);
}

/////////////////////////////////////////////////////////////////
void EXTI0_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line0) != RESET)
	{
		button_situation^=1;
		if (button_situation==1)
			inputToSystem=1;
		else
			inputToSystem=0;
		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}
/////////////////////////////////////////////////////////////////
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
