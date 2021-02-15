#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure; //Peripheral Library is activated
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;

void define_pins(){
	// PORTB is activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE); 
	
	//B10 Pin Output Configuration for RED LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PIN 10 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B1 Pin Output Configuration for Yellow LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PIN 1 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B0 Pin Output Configuration for Green LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PIN 0 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B11 Pin Input Configuration for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PIN 11 Selected
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //Input with Pull down
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
}
void normal_situation(){
	GPIO_SetBits(GPIOB, GPIO_Pin_10); //turn on red led
	delayMs(10000);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);  //turn off red led
	
	GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
	delayMs(2000);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
	
	GPIO_SetBits(GPIOB, GPIO_Pin_0); //turn on green led
	delayMs(10000);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0); //turn off green led
	
	
	
}

void define_interrupts(){
	
	// Enable clocks AFIO (for exti)
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	
	// Configure EXTI line (PB11)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
	// Configure external interrupt 
	//EXTI_DeInit(); 
	EXTI_InitStructure.EXTI_Line = EXTI_Line11; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure);
	
	// Configure nested vector interrupt controller 
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
	
}
int main(void){
	int state;
	
	define_pins(); //Pin definition function is called
	define_interrupts();
	delayInit();
	state=0;
	
	while(1) 
	{
		switch(state) //Introducing the Finite State Design
			{
			case 0:			
				normal_situation();
				break;
			
			
			}
	}
}

/////////////////

#include "stm32f10x.h"
#include "delay.h"

EXTI_InitTypeDef EXTI_InitStructure;
GPIO_InitTypeDef GPIO_InitStructure; //Peripheral Library is activated
NVIC_InitTypeDef NVIC_InitStructure;

void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void normal_situation(void);

int main(void){
	RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE); 
	
	configureGPIO();
	configureEXTI();
	configureNVIC();
	
	delayInit();
	while(1)
	{
			//normal_situation();
	}
	
}

void configureGPIO(void){

	//B10 Pin Output Configuration for RED LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PIN 10 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B1 Pin Output Configuration for Yellow LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PIN 1 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B0 Pin Output Configuration for Green LED 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0; //PIN 0 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B11 Pin Input Configuration for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PIN 11 Selected
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD; //Input with Pull down
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource11);
	
}
void configureEXTI(void){
	//EXTI_DeInit();
	EXTI_InitStructure.EXTI_Line = EXTI_Line11; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE; 
	EXTI_Init(&EXTI_InitStructure);
}
void configureNVIC(void){
	NVIC_InitStructure.NVIC_IRQChannel = EXTI3_IRQn; 
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00; 
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE; 
	NVIC_Init(&NVIC_InitStructure);
}

void normal_situation(void){
	GPIO_SetBits(GPIOB, GPIO_Pin_10); //turn on red led
	delayMs(10000);
	GPIO_ResetBits(GPIOB, GPIO_Pin_10);  //turn off red led
	
	GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
	delayMs(2000);	
	GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
	
	GPIO_SetBits(GPIOB, GPIO_Pin_0); //turn on green led
	delayMs(10000);
	GPIO_ResetBits(GPIOB, GPIO_Pin_0); //turn off green led
	
}

