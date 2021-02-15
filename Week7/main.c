#include "stm32f10x.h"
#include "delay.h"
EXTI_InitTypeDef EXTI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);

void red(void); // Decleration of red blink
void yellow(void); //Decleration of yellow blink
void green(void); ////Decleration of green blink

static uint8_t state=0; //state for switch case
static uint8_t red_green_status; //for determination of the situation after yellow blink (red or green)

int main(void)
{
  // Enable clocks for PortA (for button), PortC (for LED) and AFIO (for exti)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	
	// Configure general purpose input/output
	configureGPIO();
	// Configure external interrupt
	configureEXTI();
	// Configure nested vector interrupt controller
	configureNVIC();
	
	delayInit();

  while(1){
		switch(state)
			{
			case 0:
				red();
				break;
			case 1:
				yellow();
				break;
			case 2:
				green();
				break;
			}	
		}	
}

void configureGPIO(void) //GPIO Configuration
{
  // Configure input (BUTTON @ PA0)
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
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
	
	
  // Configure EXTI line (@PA0)
  GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
}

void configureEXTI(void)
{
  EXTI_InitStructure.EXTI_Line = EXTI_Line0; //Line zero for PA0 input
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling; 
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
  NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; // EXTI0 For PA0 input
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //High Priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

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





