#include "stm32f10x.h"
#include <math.h>
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
ADC_InitTypeDef ADC_InitStructure;
USART_InitTypeDef USART_InitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM3(void);
void configureADC1(void);
void CONFIG_UART1(void);

int8_t i = 0; 	 //It specifies the decrement(i=-1) or increment(i=1) situation on t value when buttons pressed
uint16_t j = 0;  //State variable. If button is hold j=1; if released j=0
int16_t t = 0;   //t=3*velocity value. It is used to control the brightness
unsigned int t1; //green led brightness 0<=t1<=100
unsigned int t2; //yellow led brightness 0<=t2<=100
unsigned int t3; //red led brightness 0<=t2<=100
int adc_value;   //Raw value read from the potentiometer 0<adc_value<4096
float intermediate_value; //It is an auxilary variable to calculate limit value in int type
int limit_value; //It is the limit value that limited t to increase. t can never be t>limit_value
double round (double a); //Used to specify limit value in int type 
uint16_t k; //a counter which is used to provide 0.25s period

char hiz[20];

int main(void)
{
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB, ENABLE); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO | RCC_APB2Periph_ADC1 , ENABLE); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	configureGPIO();
	configureEXTI();
	configureNVIC();
	configureTIM3();
	configureADC1();
  CONFIG_UART1();

  while(1)
		{
			adc_value = (int)(ADC_GetConversionValue(ADC1)); //0<int_value<4096
			intermediate_value=adc_value*3/4*0.01; //0<float_value<30
			limit_value=round(intermediate_value)*10;//0<int_value*10<300(+-10 incremental/decremental)
		
			if(t>=0 && t<=100)	
				{
				
				if(t<=limit_value)
					t1=t;//green leds brightness=t;
				else if(t>limit_value){
					t1=limit_value;//green leds brightness=limited_value;
					i=0;
					j=1; //t is independently equalized to limited value. see 191-200 in stmt2f10x_it.c }

				t2=0;
				t3=0;}
			}
			if(t1==100 && t<=200){
				
				if(t<=limit_value)
					t2=t-100;//yellow leds brightness=t-100;
				else if(t>limit_value){
					t2=limit_value-100;//yellow leds brightness=limited_value-100;
					i=0;
					j=1;}
				
				t1=100;
				t3=0;}
			
			if(t2==100 && t<=300){
				
				if(t<=limit_value)
					t3=t-200;//green leds brightness=t-200;
				else if(t>limit_value){
					t3=limit_value-200;//green leds brightness=limited_value-200;
					i=0;
					j=1;}
				
				t1=100;
				t2=100;
			}
			
			TIM3->CCR1 =t1;
			TIM3->CCR2 =t2;
			TIM3->CCR3 =t3;
			sprintf(hiz,"%d\r",t/3); //hiz value is mapped from the t value
		}
}
	

void configureGPIO(void){
	// Configure input PA1 (Analog Input)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure input (BUTTON @ PA0)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure input (BUTTON @ PA4)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output PA6 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configure output PA7 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configure output PB0 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  //Configure EXTI line for PA0 and PA4
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
	
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

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line4; // line 4 for PA4
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // line 1 for PA1
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}
	
void configureTIM3(void)
	{
	//Configure timer3 and set PWM frequency(100 Hz)
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
	//Init output channel 2
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); //OC Channel 2
	//Init output channel 3
	TIM_OC3Init(TIM3, &TIM_OCInitStructure); //OC Channel 3
	//Init output channel 4
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); //OC Channel 4
	
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
	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}



