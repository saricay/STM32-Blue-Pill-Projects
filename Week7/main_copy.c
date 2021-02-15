#include "stm32f10x.h"
#include "delay.h"
EXTI_InitTypeDef EXTI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);

int main(void)
{
	int state=0;
  // Enable clocks for PortA (for button), PortC (for LED) and AFIO (for exti)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
	
	// Configure general purpose input/output
	configureGPIO();
	// Configure external interrupt
	configureEXTI();
	// Configure nested vector interrupt controller
	configureNVIC();
	
	delayInit();

  while(1){
		//state=led_status();
		switch(state)
			{
			case 0:
				GPIO_SetBits(GPIOB, GPIO_Pin_10); //turn on red led
				delayMs(200);
				GPIO_ResetBits(GPIOB, GPIO_Pin_10); //turn off red led
				state=1;
				break;
			case 1:
				GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
				delayMs(200);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
				state=2;
				break;
			case 2:
				GPIO_SetBits(GPIOB, GPIO_Pin_0); //turn on green led
				delayMs(5000);
				GPIO_ResetBits(GPIOB, GPIO_Pin_0); //turn off green led
				state=3;
				break;
			case 3:
				GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
				delayMs(200);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
				state=0;
				break;
			
		}
	
	}
		
}

void configureGPIO(void)
{
  // Configure input (button)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
  // Configure output (LED)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
	// Configure output (LED @PB10)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Configure output (LED @PB1)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	// Configure output (LED @PB0)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	
  // Configure EXTI line (PA0)
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
}

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line0;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

