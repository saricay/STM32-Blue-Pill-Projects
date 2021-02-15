/*
Read value from potentiometer
Sent the value to PC via UART
Use any terminal program to see what UART is sending (I used terminal program downloaded from https://sites.google.com/site/terminalbpp/ ).
Sent 0 or 1 from terminal on PC to MC via UART
if adc value is bigger than 2000 and 1 is sent, LED will be ON.
if adc value is smaller than 2000 and 0 is sent, LED will be OFF.
*/

#include "stm32f10x.h"
#include "config.h"
#include "delay.h"
#include <stdio.h>

static int data=0;
char adc[20];
int Sent_data=0;

int main(void)
{ 
	//configuration functions are defined in config.c
	CONFIG_GPIO();
	CONFIG_UART1();
	CONFIG_ADC1();
	
	delayInit();
	
	while(1)
	{		
			data=READ_ADC(); //Data read from ADC (potentiometer)
			Sent_data=USART_ReceiveData(USART1); //Data sent from PC terminal
			if(data>2000 && Sent_data=='1')
				GPIO_SetBits(GPIOA,GPIO_Pin_1);
			if(data<=2000 && Sent_data=='0')
				GPIO_ResetBits(GPIOA,GPIO_Pin_1);
      sprintf(adc,"%d\r",data);
      UART_Transmit(adc);
			delayMs(1000);		
	}	
}

void CONFIG_GPIO(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //LED connected to PA1 pin (you can also use built in LED - PC13)
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
 // Configure input PA0 for ADC(Analog Input - Potentiometer's middle leg)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
		// Configue UART RX - UART module's TX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configue UART TX - UART module's RX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

void CONFIG_UART1(void)
{
	USART_InitTypeDef USART_InitStructure;
//  USART_ClockInitTypeDef USART_ClockInitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void CONFIG_ADC1(void)
{
	ADC_InitTypeDef ADC_InitStructure;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
  ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
  ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
  ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_InitStructure.ADC_ScanConvMode=DISABLE;
  ADC_Init(ADC1, &ADC_InitStructure);
	
  ADC_Cmd(ADC1, ENABLE);

  ADC_ResetCalibration(ADC1);
  while(ADC_GetResetCalibrationStatus(ADC1));
  ADC_StartCalibration(ADC1);
  while(ADC_GetCalibrationStatus(ADC1));

	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
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
int READ_ADC(void)
{
	 ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
   ADC_SoftwareStartConvCmd(ADC1,ENABLE);
   while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)==RESET);
   return ADC_GetConversionValue(ADC1);
}

