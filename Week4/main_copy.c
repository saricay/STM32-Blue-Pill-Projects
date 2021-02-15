#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;
int main(void){
	int button1;
	int button2;
	int i=0;
	int j=0;
	
	//RCC->APB2ENR|= (0x1<<3); //IOPB activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	//RCC->APB2ENR|= (0x1<<2); //IOPA activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	
	/*
	GPIOB->CRL &= ~(0xF<<4); //B1 output reset
	GPIOB->CRH &= ~(0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~(0xF<<8);//A10 input for button2 reset
	
	GPIOB->CRL |= (0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= (0x8<<28); //B15 input with pull-up/pull-down
	GPIOA->CRH |= (0x8<<8); //A10 input with pull-up/pull-down
	*/
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; 
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; 
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; 
	//GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; 
	//GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN; 
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	delayInit();
	//GPIOB->BRR |=(1<<1);
	GPIO_ResetBits(GPIOB, GPIO_Pin_1);
	
	while(1)
	{
		//button1 = GPIOB ->IDR |= (1<15); //Read button 1 @B15
		//button2 = GPIOA ->IDR |= (1<10); //Read button 2 @A10
		button1= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15);
		button2= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10);
		
		if ((button1 == 0) && (i==0) &&(j==0) ){
		//if ((button1 < 0x8000) && (i==0) &&(j==0) ){
			i=0;
			j=500;
		}
		else if ((button1== 1) && (i==0) &&(j==500) ){
		//else if ((button1 > 0x8000) && (i==0) &&(j==500) ){
			i=500;
		}
		else if ((button2== 1) && (i==500) && (j==500) ){
		//else if ((button2>0xBF00) && (i==500) && (j==500) ){
			i=250;
			j=250;
			delayMs(500);
		}
		else if ((button2== 1) && (i==250) && (j==250) ){
		//else if ((button2>0xBF00) && (i==250) && (j==250) ){
			i=500;
			j=500;
			delayMs(500);
		}
		else if ((button1== 1) && (i==500) &&(j==500) ){
		//else if ((button1 > 0x8000) && (i==500) &&(j==500) ){
			i=0;
			j=500;
			delayMs(1000);
		}
		
		//GPIOB->BSRR |=(1<<1);
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		delayMs(i);
		
		//GPIOB->BRR |=(1<<1);
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
		delayMs(j); 

	}
}

////////////////////////
//with register

#include "stm32f10x.h"
#include "delay.h"

int main(void){
	int button1;
	int button2;
	int i=0;
	int j=0;
	
	RCC->APB2ENR|= (0x1<<3); //IOPB activated
	RCC->APB2ENR|= (0x1<<2); //IOPA activated
	
	GPIOB->CRL &= ~(0xF<<4); //B1 output reset
	GPIOB->CRH &= ~(0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~(0xF<<8);//A10 input for button2 reset
	
	GPIOB->CRL |= (0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= (0x8<<28); //B15 input with pull-up/pull-down
	GPIOA->CRH |= (0x8<<8); //A10 input with pull-up/pull-down
	
	delayInit();
	GPIOB->BRR |=(1<<1);
	
	while(1)
	{
		button1 = GPIOB ->IDR |= (1<15); //Read button 1 @B15
		button2 = GPIOA ->IDR |= (1<10); //Read button 2 @A10
		
		/*
		if(button1>0x8000){
			GPIOB->BSRR |=(1<<1);
			delayMs(500);
			
			GPIOB->BRR |=(1<<1);
			delayMs(500); 
		}
		
		if(button2>0xBF00){
			GPIOB->BSRR |=(1<<1);
			delayMs(250);
			
			GPIOB->BRR |=(1<<1);
			delayMs(250); 
		}
		*/
		
		if ((button1 < 0x8000) && (i==0) &&(j==0) ){
			i=0;
			j=500;
		}
		else if ((button1 > 0x8000) && (i==0) &&(j==500) ){
			i=500;
		}
		else if ((button2>0xBF00) && (i==500) && (j==500) ){
			i=250;
			j=250;
			delayMs(500);
		}
		else if ((button2>0xBF00) && (i==250) && (j==250) ){
			i=500;
			j=500;
			delayMs(500);
		}
		
		else if ((button1 > 0x8000) && (i==500) &&(j==500) ){
			i=0;
			j=500;
			delayMs(1000);
		}
		
		GPIOB->BSRR |=(1<<1);
		delayMs(i);
		
		GPIOB->BRR |=(1<<1);
		delayMs(j); 

	}
}


// fina peripheral
#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;
int main(void){
	int button1;
	int button2;
	int i=0; //Open time for LED in Ms, Default Value
	int j=0; //Closed time for LED in MS, Default Value
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // PORTB activated
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE); // PORTA activated
	
	//B1 Pin Output Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1; //PIN 1 Selected
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz; //Frequency of Output, LED
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; //Pull Up/Pull Down Mode Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//B15 Pin Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15; //PIN 15 Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
	
	//A10 Pin Configuration
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PIN 10 Selected
	GPIO_Init(GPIOA, &GPIO_InitStructure); //PIN is selected for PORT A
	
	delayInit(); 
	GPIO_ResetBits(GPIOB, GPIO_Pin_1); //At first, reset to output pin @B1
	
	while(1)
	{
		button1= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_15); //button1's value which is 1 when button is pressed, 0 when not pressed.
		button2= GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_10); //button2's value which is 1 when button is pressed, 0 when not pressed.
		
		if ((button1 == 0) && (i==0) &&(j==0) ){ // @Inititial position, button is off
			i=0; //On time for led
			j=500; //Off time for led
			delayMs(100); // Wait for a bit
		}
		else if ((button1== 1) && (i==0) &&(j==500) ){ // When button1 is pressed to open LED from OFF to ON
			i=500; // i+j=1000 Ms Period
			delayMs(100); 
		}
		else if ((button2== 1) && (i==500) && (j==500) ){ // When LED's period is 1000Ms, button2 change the period as 500Ms
			i=250;
			j=250;
			delayMs(200); // Wait a bit to sense the button truly
		}
		else if ((button2== 1) && (i==250) && (j==250) ){ //When LED's period is 500Ms, button2 change the period as 1000Ms
			i=500;
			j=500;
			delayMs(200); // Wait a bit to sense the button truly
		}
		else if ((button1== 1) && (i==500) &&(j==500) ){ //When LED's period is again 1000 Ms, button1 turns the LED off 
			i=0;
			j=500;
			delayMs(100);
		}
		
			GPIO_SetBits(GPIOB, GPIO_Pin_1); //Give Output as 1 to the pin B1
			delayMs(i); //On time for LED
			
			GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Give Output as 1 to the pin B1
			delayMs(j); //Off time for LED
	}
}

//
#include "stm32f10x.h"
#include "delay.h"

int main(void){
	unsigned long button1; 
	unsigned long button2; 
	int i=0; //Open time for LED in Ms, Default Value
	int j=0; //Closed time for LED in MS, Default Value
	
	RCC->APB2ENR|= (0x1<<3); //IOPB activated
	RCC->APB2ENR|= (0x1<<2); //IOPA activated
	
	GPIOB->CRL &= ~(0xF<<4); //B1 output reset
	GPIOB->CRH &= ~(0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~(0xF<<8);//A10 input for button2 reset
	
	GPIOB->CRL |= (0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= (0x8<<28); //B15 input with pull-up/pull-down
	GPIOA->CRH |= (0x8<<8); //A10 input with pull-up/pull-down
	
	delayInit();
	GPIOB->BRR |=(1<<1); // Reset B1 output value at first
	
	while(1)
	{
		button1 = GPIOB ->IDR |= (1<15); //Button1's output threshold value when pressed
		button2 = GPIOA ->IDR |= (1<10); //Button2's output threshold value when pressed 
		
		//I could not handle with if ( GPIOx->IDR && (1<<y) ) situation. 
		//Therefore I find this solution. When buttons are pressed, their values
		//are higher than those threshold values
		
		if ((button1 < 0x8000) && (i==0) &&(j==0) ){ // @Inititial position, button is off
			i=0;
			j=500;
		}
		else if ((button1 > 0x8000) && (i==0) &&(j==500) ){ // When button1 is pressed to open LED from OFF to ON
			i=500;
		}
		else if ((button2>0xBF00) && (i==500) && (j==500) ){ // When LED's period is 1000Ms, button2 change the period as 500Ms
			i=250;
			j=250;
			delayMs(200);
		}
		else if ((button2>0xBF00) && (i==250) && (j==250) ){ //When LED's period is 500Ms, button2 change the period as 1000Ms
			i=500;
			j=500;
			delayMs(200);
		}
		else if ((button1 > 0x8000) && (i==500) &&(j==500) ){ //When LED's period is again 1000 Ms, button1 turns the LED off 
			i=0;
			j=500;
			delayMs(100);
		}
		
		GPIOB->BSRR |=(1<<1); //Give Output as 1 to the pin B1
		delayMs(i); //On time for LED
		
		GPIOB->BRR |=(1<<1); //Give Output as 1 to the pin B1
		delayMs(j); //Off time for LED
	}
}

//
#include "stm32f10x.h"
#include "delay.h"

int main(void){
	int i=0;
	int j=0;
	RCC->APB2ENR|= (0x1<<3); //IOPB activated
	
	GPIOB->CRL &= ~((uint32_t)0xF<<4); //B1 output reset
	GPIOB->CRH &= ~((uint32_t)0xF<<28);//B15 input for button1 reset

	GPIOB->CRL |= ((uint32_t)0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= ((uint32_t)0x8<<28); //B15 input with pull-up/pull-down

	delayInit();
	
	while(1)
	{
		if ( GPIOB->IDR & (0x1<<15)  ){ // if B15 is pressed, B1 blinking
			GPIOB->BSRR |=(0x1<<1);
			delayMs(500);
			GPIOB->BRR |=(0x1<<1);
			delayMs(500);
		 }

	}
}



