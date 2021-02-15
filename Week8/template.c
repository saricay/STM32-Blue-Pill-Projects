#include "stm32f10x.h"

EXTI_InitTypeDef EXTI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM2(void);

int statusLED = 0;
int main(void)
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	// Configure general purpose input/output
	configureGPIO();
	// Configure external interrupt
	//configureEXTI();
	// Configure nested vector interrupt controller
	configureNVIC();
	
	configureTIM2();

  while(1){
		if(statusLED)
			GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		else
			GPIO_SetBits(GPIOC, GPIO_Pin_13);
		}	
}	

void configureGPIO(void) //GPIO Configuration
{
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

}

void configureTIM2(void)
{
	TIM_TimeBaseStructure.TIM_Period = 49999;
	TIM_TimeBaseStructure.TIM_Prescaler= 143;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable;
	TIM_OCInitStructure.TIM_Pulse = 25000;
	TIM_OC1Init(TIM2, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}
/*
void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; //Line zero for PA0 input
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}
*/
void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn; // EXTI0 For PA0 input
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //High Priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

/*
void red(){
GPIO_SetBits(GPIOB, GPIO_Pin_10); //turn on red led
delayMs(10000); // 10 seconds for RED
GPIO_ResetBits(GPIOB, GPIO_Pin_10);//turn off red led
red_green_status=0; // If it 0, green blink comes after yellow blink
state=1;	//go to yellow state
}

void yellow(){
GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
delayMs(2000); // 2 seconds for YELLOW
GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
if (red_green_status==0) state=2; //go to green state
else if (red_green_status==1) state=0; //go to red state
}

void green(){
GPIO_SetBits(GPIOB, GPIO_Pin_0); //turn on green led
delayMs_2(10000); // 10 seconds for GREEN, special function @delay.c file
GPIO_ResetBits(GPIOB, GPIO_Pin_0); //turn off green led
red_green_status=1;	// If it 1, red blink comes after yellow blink
state=1; //go to yellow state
}

*/



