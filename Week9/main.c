#include "stm32f10x.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM2(void);
void configureTIM3(void);
void configureTIM4(void);

uint8_t i = 0; 
uint16_t j = 0;
uint8_t t = 1;
static int t2=0;
static int t3=0;
static int t4=0;

int main(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4,ENABLE ); //Timer Interrupts 2,3,4 Activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //Port B activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE); //Port A and Alternate Function IO is activated
	
	// Configure general purpose input/output
	configureGPIO();
	// Configure external interrupt
	configureEXTI();
	// Configure nested vector interrupt controller
	configureNVIC();
	
	//Configure timer interrupt
	configureTIM2();
	configureTIM3();
	configureTIM4();

  while(1){
		
		if(i==0){
			//state0: Off time for all
			TIM2->CCR2 =0;
			TIM3->CCR2 =0;
			TIM4->CCR2 =0;
			t=1;
		}
		
		if(i==1){
			//state1: On time for RED LED 
			TIM2->CCR2 =t; 
			TIM3->CCR2 =0;
			TIM4->CCR2 =0;

		}
		if(i==2){
			//state2: On time for for YELLOW LED
			TIM2->CCR2 =0;
			TIM3->CCR2 =t; 
			TIM4->CCR2 =0;
		}
		
		if(i==3){
			//state3: On time for GREEN LED
			TIM2->CCR2 =0;
			TIM3->CCR2 =0;
			TIM4->CCR2 =t; 
		}
	}	
}
void configureGPIO(void) //GPIO Configuration
{
	// Configure input (BUTTON @ PA0)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure input (BUTTON @ PA4)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output (LED @ PA1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output (LED @ PA7)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output (LED @PB7)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  //Configure EXTI line for PA0 and PA4
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource4);
	
}

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line4; // line 4 for PA4
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line0; // line 1 for PA1
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; 
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

void configureTIM2(void)
	{ //Red led is connected to TIM2
	//Configure timer2 and set PWM frequency(1 kHz)
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler= 719;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM2_CCMR1
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM2_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM2_CCER
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = t2; //TIM2_CCR2 (%duty) It specifies red led's on period
	//Set initial PWM duty cycle
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); //OC Channel 2
	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
	
}
	
void configureTIM3(void)
	{ //Yellow led is connected to TIM3
	//Configure timer3 and set PWM frequency(1 kHz)
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler= 719;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM3_CCMR1
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM3_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM3_CCER
		
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = t3; //TIM3_CCR2 (%duty) It specifies yellow led's on period
	//Set initial PWM duty cycle
	TIM_OC2Init(TIM3, &TIM_OCInitStructure); //OC Channel 2
	TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	TIM_Cmd(TIM3, ENABLE);
}
	
void configureTIM4(void)
	{ //Green led is connected to TIM4
	//Configure timer2 and set PWM frequency(1 kHz)
	TIM_TimeBaseStructure.TIM_Period = 99;
	TIM_TimeBaseStructure.TIM_Prescaler= 719;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM4_CCMR1
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM4_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM4_CCER
		
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = t4; //TIM4_CCR2 (%duty) It specifies green led's on period
	//Set initial PWM duty cycle
	TIM_OC2Init(TIM4, &TIM_OCInitStructure); //OC Channel 2
	TIM_ITConfig(TIM4, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	TIM_Cmd(TIM4, ENABLE);
}

