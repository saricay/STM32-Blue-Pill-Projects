#include "stm32f10x.h"
#include "math.h"

GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
ADC_InitTypeDef ADC_InitStructure;

void configureGPIO(void);
void configureTIM2(void);
void configureTIM3(void);
void configureADC1(void);
void configureNVIC(void);

float f=0; // f is the frequency of the sinusoidal signal. Set 0 firstly.
float i=0; // i is the raw value read by ADC. Set 0 fistly.
int j=0;   // 1/200 of the i value. It is used to increment the value of frequency.
float t=0; // Current time variable. Initially set 0.

int main(void)
{	
	//Set ADC Clock (Max 14Mhz  | 72/6=12 MHz)
	RCC_ADCCLKConfig(RCC_PCLK2_Div6);
	
	//Enable clocks for PortA(LED& AIN), PORTB(LED) and TIM2,TIM3
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE ); 
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE ); 
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_ADC1 | RCC_APB2Periph_AFIO, ENABLE); //Port A,B and Alternate Function IO is activated

	// Configure general purpose input/output
	configureGPIO();
	
	//Configure timer interrupts
	configureTIM2(); //To use 
	configureTIM3();
	configureNVIC();
	
	//Configure ADC channel
	configureADC1();
	
  while(1)
	{	
		//configureTIM2();
		i= (int)(ADC_GetConversionValue(ADC1)); //theoretically 0<i<4096 in integer numbers
		j=i/200; // 0<j<20 in integer numbers
		f=0.4+(float)j/10; // 0.4<frequency<2.4 Hz with the resolution 0.1 Hz
		
		//To reduce the noise in low frequency, some mean values set in a spesific interval
		if (f>=0.5 && f<= 0.8)
			f=0.7;
		if (f>=0.9 && f<=1.1)
			f=1.0;
		
		TIM3->CCR1 = (sin(2*3.14*f*t+3.14/2/f)+1)/2 * 36000; //Phase is +90/f degree
		TIM3->CCR2 = (sin(2*3.14*f*t+3.14/3/f)+1)/2 * 36000; //Phase is +60/f degree
		TIM3->CCR3 = (sin(2*3.14*f*t+3.14/6/f)+1)/2 * 36000; //Phase is +30/f degree
		TIM3->CCR4 = (sin(2*3.14*f*t)+1)/2 * 36000; 				 //Phase is 0 degree
		
	}
}

void configureGPIO(void) //GPIO Configuration
{
	// Configure input PA0 (Analog Input)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
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
	// Configure output PB1 (PWM output so Alternate-Function)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
}

void configureTIM2(void)
	{ 
	
	//Configure time base generation (100 Hz)
	//Counter of the time with incremental 10 ms
	TIM_TimeBaseStructure.TIM_Period = 35999;
	TIM_TimeBaseStructure.TIM_Prescaler= 19;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//For timing purpose only
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing; //TIM2_CCMR1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM2_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM2_CCER
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	//Enable timer 2
	TIM_Cmd(TIM2, ENABLE);
}

void configureTIM3(void)
	{
	//Configure timer3 and set PWM frequency(100 Hz)
	TIM_TimeBaseStructure.TIM_Period = 35999;
	TIM_TimeBaseStructure.TIM_Prescaler= 19;
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
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

