#include "stm32f10x.h"
#include "delay.h"

int main(void){
	unsigned int button1;
	unsigned int button2;
	//int i=0;
	//int j=0;
	
	RCC->APB2ENR|= ((uint32_t)0x1<<3); //IOPB activated
	RCC->APB2ENR|= ((uint32_t)0x1<<2); //IOPA activated
	
	GPIOB->CRL &= ~((uint32_t)0xF<<4); //B1 output reset
	GPIOB->CRH &= ~((uint32_t)0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~((uint32_t)0xF<<8); //A10 input for button2 reset
	
	GPIOB->CRL |= ((uint32_t)0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= ((uint32_t)0x8<<28); //B15 0x4:floating input(reset state)/0x8: Input with pull-up,pull-down 
	GPIOA->CRH |= ((uint32_t)0x8<<8); //A10  0x4:floating input(reset state)/0x8: Input with pull-up,pull-down 
	
	delayInit();
	GPIOB->BRR |=(1<<1);
	
	while(1)
	{
		button1 = GPIOB ->IDR |= (1<15); //Read button 1 @B15
		button2 = GPIOA ->IDR |= (1<10); //Read button 2 @A10
		
		if(button1==(uint32_t)0x8000){
			GPIOB->BSRR |=(1<<1);
			delayMs(500);
			
			GPIOB->BRR |=(1<<1);
			delayMs(500); 
		}
		
		if(button2==(uint32_t)0x0400){
			GPIOB->BSRR |=(1<<1);
			delayMs(250);
			
			GPIOB->BRR |=(1<<1);
			delayMs(250); 
		}
		/*
		
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
*/
	}
}


