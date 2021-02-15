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

long int t = -1; //time variable initialized
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
		if(t>=0 && t<10){
			//state0
			GPIO_ResetBits(GPIOA, GPIO_Pin_6); //Turn off red for people
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Turn off yellow for cars
			GPIO_SetBits(GPIOB, GPIO_Pin_10);//Red for cars
			GPIO_SetBits(GPIOA, GPIO_Pin_7);//Green for people
		}
		if(t>=10 && t<12){
			//state1
			GPIO_ResetBits(GPIOB, GPIO_Pin_10); //Turn off red for cars
			GPIO_ResetBits(GPIOA, GPIO_Pin_7); //Turn off green for people
			GPIO_SetBits(GPIOB, GPIO_Pin_1); //Yellow for cars
			GPIO_SetBits(GPIOA, GPIO_Pin_6); //Red for people
		}
		if(t>=12 && t<22){
			//state2
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Turn off yellow for people
			GPIO_SetBits(GPIOA, GPIO_Pin_6); //Red for people
			GPIO_SetBits(GPIOB, GPIO_Pin_0); //Green for cars
		}
		if(t>=22 && t<24){
			//state3
			GPIO_ResetBits(GPIOB, GPIO_Pin_0); //Turn off green for cars
			GPIO_SetBits(GPIOB, GPIO_Pin_1); //Yellow for cars
		}
		
		}	
}	

void configureGPIO(void) //GPIO Configuration
{
	// Configure input (BUTTON @ PA0)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	///////LEDS FOR CAR/////////////
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
	
	///////LEDS FOR PEOPLE/////////////
	// Configure output (GREEN LED @PA7)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configure output (RED LED @PA6)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	
  // Configure EXTI line for (@PA0)
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);

}

void configureTIM2(void)
	{ //Definition of 1 Hz: 72000000/(50000*1440)
	TIM_TimeBaseStructure.TIM_Period = 49999;
	TIM_TimeBaseStructure.TIM_Prescaler= 1439;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);
	
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, ENABLE);
}

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; // line 0 for A0
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
}





