#include "stm32f10x.h"
#include <math.h>
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
//////////////////////////////////////////////////////////////////
void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM3(void);
//////////////////////////////////////////////////////////////////
float t;
// Step size to solve ODE 
volatile double stepSize = 0.01; 
// System input (step) to be sent if exti triggered 
volatile double inputToSystem = 0.0; 
// System parameters 
double k = 1, tau = 0.4;
volatile double outputToPC;
int i;
/////////////////////////////////////////////////////////////////

int main(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO ,ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE );
	
	configureGPIO();
	configureEXTI();
	configureNVIC();
	configureTIM3();
	
	while(1){
	}
}

/////////////////////////////////////////////////////////////////

// Differential Equation y'(t) = f(y,u,t) 
double diffEqn(double y, double u) {
		return (k / tau) * u - (1 / tau) * y; 
}

// Explicit Euler ODE Solver 
double ExplicitEuler(double (*f)(double,double), double ts, double uk) {
	static double yk = 0; // Previous value of the output 
	double yk1; // System output 
	yk1 = yk + ts * f(yk, uk); 
	yk = yk1;
	return yk1;
}
////////////////////////////////////////////////////////////////
void configureGPIO(void)
{
	//Configure input(button)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//Configure EXTI line(PA0)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
}
void configureEXTI(void){
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void configureTIM3(void)
	{
	//Configure timer base generation
	TIM_TimeBaseStructure.TIM_Period = 399;
	TIM_TimeBaseStructure.TIM_Prescaler= 1799;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//Configure output compare mode
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing; //TIM3_CCMR1
	//TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM3_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Disable; //TIM3_CCER
	TIM_OCInitStructure.TIM_Pulse = 0; //TIM3_CCR1
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update | TIM_IT_CC1, ENABLE);
	//Enable timer 3
	TIM_Cmd(TIM3, ENABLE);
}
	
