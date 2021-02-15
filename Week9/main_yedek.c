#include "stm32f10x.h"

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void configureGPIO(void);//+
void configureEXTI(void);//+
void configureNVIC(void);//+
void configureTIM2(void);

int t = 0; //time variable initialized

int main(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE); //Timer Interreupt Activated
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

  while(1){
		/*
		if(t==0){
			//state0
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); 
			GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
			GPIO_ResetBits(GPIOB, GPIO_Pin_10);
		}
		
		if(t==1){
			//state1
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); 
			GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
			GPIO_SetBits(GPIOB, GPIO_Pin_10);

		}
		if(t==2){
			//state2
			GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
			GPIO_ResetBits(GPIOB, GPIO_Pin_10); 
			GPIO_SetBits(GPIOB, GPIO_Pin_1);
		}
		
		if(t==3){
			//state3
			GPIO_ResetBits(GPIOB, GPIO_Pin_0); 
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); 
			GPIO_SetBits(GPIOB, GPIO_Pin_0);
		}
		*/
		}	
}	

void configureGPIO(void) //GPIO Configuration
{
	// Configure input (BUTTON @ PA0)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure input (BUTTON @ PA1)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output (RED LED @PB10)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Configure output (YELLOW LED @PB1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Configure output (GREEN LED @PB0)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
  // Configure EXTI line for PA0 and PA1
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource1);
}

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; // line 0 for PA0
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line1; // line 1 for PA1
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
	
	NVIC_InitStructure.NVIC_IRQChannel = EXTI1_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void configureTIM2(void)
	{ 
	//Configure timer2 and set PWM frequency(1 Hz)
	TIM_TimeBaseStructure.TIM_Period = 9999;
	TIM_TimeBaseStructure.TIM_Prescaler= 7199;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	//Configure output compare (PWM) mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM2_CCMR1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM2_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM2_CCER
		
	//Set Initial PWM duty cycle
	TIM_OCInitStructure.TIM_Pulse = 0; //TIM2_CCR2 (%duty)
	//Set initial PWM duty cycle
	TIM_OC2Init(TIM2, &TIM_OCInitStructure); //OC Channel 2
		
	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}






