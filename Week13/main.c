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
void CONFIG_UART1(void);
void configureI2C(void);
void read_sensor(void);

int adc_value;   //Raw value read from the potentiometer 0<adc_value<4096
float intermediate_value; //It is an auxilary variable to calculate limit value in int type
int limit_value; //If temperature> limit value && a=1 => LED ON
double round (double a); //Used to specify limit value in int type 

char hiz[20];
uint16_t dataBuffer[20];
int statusLED; //It is compulsory variable to be set 1 if LED is desired to turn ON
							 //It is sent from the PC, via terminal.

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
  CONFIG_UART1();
	configureI2C();

  while(1)
		{
			adc_value = (int)(ADC_GetConversionValue(ADC1)); //0<(int)adc_value<4096
			intermediate_value=adc_value*1/80; // Data with noise
			limit_value=round(intermediate_value); // Data with noise reduced. 0<(int)limit_value<50 (+1 incremental)
			read_sensor();
			
			if(dataBuffer[0]>limit_value && statusLED==1){
				//Both conditions have to be done to turn LED on
				GPIO_SetBits(GPIOA, GPIO_Pin_6);
			}
			else if(dataBuffer[0]<limit_value || statusLED==0){
				//One of the conditions is enough to be done if LED is desired in OFF condition
				GPIO_ResetBits(GPIOA, GPIO_Pin_6);}
				
			sprintf(hiz,"%d\r",dataBuffer[0]); //Send LSB of temperature value. 
																				 //MSB is not required since we are not dealing with negative Temp values 
		}
}
	

void configureGPIO(void){
	// Configure input PA1 (Analog Input)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	
	// Configure output PA6 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
		// Configue UART RX - UART module's TX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
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
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);

}
	
void configureTIM3(void)
	{
	//Configure timer3 and set PWM frequency(400 Hz)
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler= 1799;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM3_CCMR1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM3_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM3_CCER
		
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = 0; //TIM3_CCR1 (%duty)
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

void CONFIG_UART1(void)
{
//  USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
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


