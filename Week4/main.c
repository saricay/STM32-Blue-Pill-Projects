#include "stm32f10x.h"
#include "delay.h"

int main(void){
	int a;
	int i=0; //Open time for LED in Ms, Default Value
	int j=0; //Closed time for LED in MS, Default Value
	
	RCC->APB2ENR|= (0x1<<3); //IOPB activated
	RCC->APB2ENR|= (0x1<<2); //IOPA activated
	
	GPIOB->CRL &= ~((uint32_t)0xF<<4); //B1 output reset
	GPIOB->CRH &= ~((uint32_t)0xF<<28);//B15 input for button1 reset
	GPIOA->CRH &= ~((uint32_t)0xF<<8);//A10 input for button2 reset
	
	GPIOB->CRL |= ((uint32_t)0x2<<4); //B1 output at 2MHz
	GPIOB->CRH |= ((uint32_t)0x8<<28); //B15 input with pull-up/pull-down
	GPIOA->CRH |= ((uint32_t)0x8<<8); //A10 input with pull-up/pull-down
	
	delayInit();
	GPIOB->BRR |=(1<<1); // Reset B1 output value at first
	
	while(1)
	{
		
		//I could not handle with if ( GPIOx->IDR && (1<<y) ) situation. 
		//Therefore I find this solution. When buttons are pressed, their values
		//are higher than those threshold values
		
		if ( (GPIOB ->IDR |= 0<<15 ) && (i==0) &&(j==0) ){ // @Inititial position, button is off
			i=0;
			j=500;
		}
		else if ( (GPIOB ->IDR & (1<<15) ) && (i==0) &&(j==500) ){ // When button1 is pressed to open LED from OFF to ON
			i=500;
		}
		else if ( (GPIOA ->IDR & (1<<10) ) && (i==500) && (j==500) ){ // When LED's period is 1000Ms, button2 change the period as 500Ms
			i=250;
			j=250;
			delayMs(200);
		}
		else if ( (GPIOA ->IDR & (1<<10) ) && (i==250) && (j==250) ){ //When LED's period is 500Ms, button2 change the period as 1000Ms
			i=500;
			j=500;
			delayMs(200);
		}
		else if ( (GPIOB ->IDR & (1<<15) ) && (i==500) &&(j==500) ){ //When LED's period is again 1000 Ms, button1 turns the LED off 
			i=0;
			j=500;
			delayMs(100);
		}
		
		for (a=0; a<2*j; a=a+1){
			if (a<j & i>0){
				GPIOB->BSRR |=(1<<1); //Give Output as 1 to the pin B1
				delayMs(1); //On time for LED
				if ( GPIOA ->IDR & (1<<10)  || (GPIOB ->IDR & (1<<15) )){
					break;
				}
			}
			else{
				GPIOB->BRR |=(1<<1); //Give Output as 1 to the pin B1
				delayMs(1); //Off time for LED
				if ( GPIOA ->IDR & (1<<10)  || (GPIOB ->IDR & (1<<15) )){
					break;
				}
			}
		}
	}
}

