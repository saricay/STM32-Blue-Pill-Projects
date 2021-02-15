#include "stm32f10x.h"
#include "delay.h"

int main(void){
	unsigned int button1;
	unsigned int button2;
	int i=0;
	int j=0;
	//int a;
	
	RCC->APB2ENR|= (0x1<<3); //IOPB activated
	RCC->APB2ENR|= (0x1<<2); //IOPA activated
	
	GPIOB->CRL &= ~(0xF<<4); //B1 output reset
	GPIOB->CRH &= ~((uint32_t)0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~(0xF<<8);//A10 input for button2 reset
	
	GPIOB->CRL |= (0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= ((uint32_t)0x8<<28); //B15 input with pull-up/pull-down
	GPIOA->CRH |= ((uint32_t)0x8<<8); //A10 input with pull-up/pull-down
	
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
		
		if ((button1 < (uint32_t)0x8000) && (i==0) &&(j==0) ){
			i=0;
			j=500;
		}
		else if ((button1 > (uint32_t)0x8000) && (i==0) &&(j==500) ){
			i=500;
			GPIOB->BSRR |=(1<<1);
		}
		else if ((button2>0xBF00) && (i==500) && (j==500) ){
			i=250;
			j=250;
			//delayMs(500);
		}
		else if ((button2>0xBF00) && (i==250) && (j==250) ){
			i=500;
			j=500;
			//delayMs(500);
		}
		
		else if ((button1 > (uint32_t)0x8000) && (i==500) &&(j==500) ){
			i=0;
			j=500;
			GPIOB->BRR |=(1<<1);
		}
		
		
		
		/*
		for(a=0; a<i; a=a+10 ){ 				
				GPIOB->BSRR |=(1<<1);
				delayMs(10);
				if( (button1 > 0x8000) ){
					break;
				}
		}
		
		for(a=0; a<j; a=a+10 ){ 
				if( (button1 > 0x8000) || (button2>0xBF00) ){
					break;
				}
				GPIOB->BRR |=(1<<1);
				delayMs(10); 
		}
		*/
	}	
}
