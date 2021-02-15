#include "stm32f10x.h"
#include <math.h>
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
ADC_InitTypeDef ADC_InitStructure;
USART_InitTypeDef USART_InitStructure;
I2C_InitTypeDef I2C_InitStructure;

void configureGPIO(void);
void configureNVIC(void);
void configureTIM3(void);
void configureADC1(void);
void configureUART1(void);
void configureI2C(void);
void read_sensor(void);

int adc_value;   //Raw value read from the potentiometer 0<adc_value<4096
char temperature[20]; //Sent data via UART
uint16_t dataBuffer[20]; //Received data from I2C
float reference_temperature;
float t; //The variable is used to adjust the frequency of the read sensor and send the value via UART
float error_percent,overshoot_percent;

int main(void)
{
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1 , ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	configureGPIO();
	configureNVIC();
	configureTIM3();
	configureADC1();
  configureUART1();
	configureI2C();

  while(1)		
		{
			adc_value = (int)(ADC_GetConversionValue(ADC1)); 
			
			//To adjust the reference temperature value
			if (adc_value<=250){ //Firstly none of the reference value is selected
				reference_temperature=dataBuffer[0]; //Cancel the voltage applied on the ceramic resistors
				GPIO_ResetBits(GPIOA, GPIO_Pin_5); 
				GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
				GPIO_ResetBits(GPIOA, GPIO_Pin_3); 
				TIM_OCInitStructure.TIM_Pulse = 1000;}
			else if (adc_value>250 && adc_value<=1333){
				reference_temperature=35; //Low reference value
				GPIO_SetBits(GPIOA, GPIO_Pin_5); //Green Led
				GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
				GPIO_ResetBits(GPIOA, GPIO_Pin_3); }
			else if(adc_value>1333 && adc_value<=2666){
				reference_temperature=50;//Medium reference value
				GPIO_ResetBits(GPIOA, GPIO_Pin_3); 
				GPIO_SetBits(GPIOA, GPIO_Pin_4); //Yellow Led
				GPIO_ResetBits(GPIOA, GPIO_Pin_5); }
			else if(adc_value>2666){
				reference_temperature=65;//High reference value
				GPIO_SetBits(GPIOA, GPIO_Pin_3); //Red Led
				GPIO_ResetBits(GPIOA, GPIO_Pin_4); 
				GPIO_ResetBits(GPIOA, GPIO_Pin_5); }
		}
}
	

void configureGPIO(void){ //6LEDs, 1pwm, 1ADC, 2 USART, 2 I2C 
	//A3 Pin Output Configuration for Red LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3; //PIN 3 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PIN is selected for PORT A
	//A4 Pin Output Configuration for Yellow LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4; //PIN 4 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PIN is selected for PORT A
	//A5 Pin Output Configuration for Green LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5; //PIN 5 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PIN is selected for PORT A
	
	//B15 Pin Output Configuration for Red LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //PIN 15 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	//B14 Pin Output Configuration for Yellow LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14; //PIN 14 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	//B13 Pin Output Configuration for Green LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13; //PIN 13 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	
	// Configure input PA1 (Analog Input)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output PA6 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configue UART TX - UART module's RX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure pins (SDA, SCL) for I2C
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x01;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
	
void configureTIM3(void)
	{
	//Configure timer3 and set PWM frequency(1 kHz)
	TIM_TimeBaseStructure.TIM_Period = 999;
	TIM_TimeBaseStructure.TIM_Prescaler= 72;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM3_CCMR1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM3_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM3_CCER
		
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = 1000; //TIM3_CCR1 (%duty)
	//Init output channel 1
	TIM_OC1Init(TIM3, &TIM_OCInitStructure); //OC Channel 1
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//Enable timer 3
	TIM_Cmd(TIM3, ENABLE);
}
	
void configureADC1(void){
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void configureUART1(void)
{
//  USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	
  //USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void configureI2C(void){
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2; 
	I2C_InitStructure.I2C_OwnAddress1 = 0x00; 
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable; 
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit; 
	I2C_InitStructure.I2C_ClockSpeed = 100000; 
	I2C_Init(I2C1, &I2C_InitStructure); I2C_Cmd(I2C1, ENABLE);
}

void read_sensor(void){
	// Wait if busy 
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)); 
	// Generate START condition 
	I2C_GenerateSTART(I2C1, ENABLE); 
	while(!I2C_GetFlagStatus(I2C1, I2C_FLAG_SB)); 
	// Send device address for read 
	I2C_Send7bitAddress(I2C1, 0x90, I2C_Direction_Receiver); 
	while (!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)); 
	// Read the first data 
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); 
	dataBuffer[0] = I2C_ReceiveData(I2C1); 
	// Disable ACK and generate stop condition 
	I2C_AcknowledgeConfig(I2C1, DISABLE); 
	I2C_GenerateSTOP(I2C1, ENABLE);
	// Read the second data 
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED)); 
	dataBuffer[1] = I2C_ReceiveData(I2C1);
}


