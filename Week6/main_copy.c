#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure;
void all_leds_on(){
		GPIO_SetBits(GPIOB, GPIO_Pin_10); //Give Output as 1 to the pin B1
		GPIO_SetBits(GPIOB, GPIO_Pin_1); //Give Output as 1 to the pin B1
		GPIO_SetBits(GPIOB, GPIO_Pin_0); //Give Output as 1 to the pin B1
	}

void all_leds_off(){
		GPIO_ResetBits(GPIOB, GPIO_Pin_10); //At first, reset to output pin @B10
		GPIO_ResetBits(GPIOB, GPIO_Pin_1); //At first, reset to output pin @B1
		GPIO_ResetBits(GPIOB, GPIO_Pin_0); //At first, reset to output pin @B0
	}

void red_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_10); //Give Output as 1 to the pin B1
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		delayUs(20);
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void yellow_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		delayUs(20);
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

void green_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		delayUs(20);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void define_pins(){
	
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
	
	//B12 Pin Input Configuration for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PIN 12 Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
}


int button;
int main(void){

	int state;
	int i;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // PORTB activated
	
	define_pins();

	delayInit(); 
	state=0;
	
	while(1)
	{
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		switch(state)
			{
			case 0:			
				all_leds_off();
				if (button==1){
					state=1;
				}
				break;
			
			case 1:
				all_leds_on();
				if (button==0){
					state=2;
				}
				break;
				
			case 2:
				for (i=0;i<25000;i=i+1){
					red_blink();
					if (button == 1){
						state=5;
					}
				}	
				if (state != 5){
					state=3;}
				break;
			
			case 3:
				for(i=0;i<25000;i=i+1){
					yellow_blink();
					if (button == 1){
						state=5;
					}
				}
				if (state != 5){
					state=4;}
				break;
				
			case 4:
				for(i=0;i<25000;i=i+1){
					green_blink();
					if (button == 1){
						state=5;
					}
				}
				if (state != 5){
					state=2;
				}
			break;
				
			case 5:
				all_leds_on();
			
				if (button==0){
					state=0;
				}
				break;
		}
	}
}

///////
#include "stm32f10x.h"
#include "delay.h"
#include <string.h>
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure;
void all_leds_on(){
		GPIO_SetBits(GPIOB, GPIO_Pin_10); //Give Output as 1 to the pin B1
		GPIO_SetBits(GPIOB, GPIO_Pin_1); //Give Output as 1 to the pin B1
		GPIO_SetBits(GPIOB, GPIO_Pin_0); //Give Output as 1 to the pin B1
	}

void all_leds_off(){
		GPIO_ResetBits(GPIOB, GPIO_Pin_10); //At first, reset to output pin @B10
		GPIO_ResetBits(GPIOB, GPIO_Pin_1); //At first, reset to output pin @B1
		GPIO_ResetBits(GPIOB, GPIO_Pin_0); //At first, reset to output pin @B0
	}

void red_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_10); //Give Output as 1 to the pin B1
		delayUs(500000);
		GPIO_ResetBits(GPIOB, GPIO_Pin_10);
}

void yellow_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_1);
		delayUs(500000);
		GPIO_ResetBits(GPIOB, GPIO_Pin_1);
}

void green_blink(){
		GPIO_SetBits(GPIOB, GPIO_Pin_0);
		delayUs(500000);
		GPIO_ResetBits(GPIOB, GPIO_Pin_0);
}

void define_pins(){
	
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
	
	//B12 Pin Input Configuration for Button 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PIN 12 Selected
	GPIO_Init(GPIOB, &GPIO_InitStructure); //PIN is selected for PORT B
}



int main(void){
	int button;
	int state;
	//int i=0; //Open time for LED in Ms, Default Value
	//int j=0; //Closed time for LED in MS, Default Value
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // PORTB activated
	
	define_pins();

	delayInit(); 
	state=0;
	
	while(1)
	{
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
		switch(state)
			{
			case 0:			
				all_leds_off();
				if (button==1){
					state=1;
				}
				break;
			
			case 1:
				all_leds_on();
				if (button==0){
					state=2;
				}
				break;
				
			case 2:
				red_blink();
				if (button == 1){
					state=5;
				}
				else{
					state=3;
				}
				break;
				
			case 3:
				yellow_blink();
				if (button == 1){
					state=5;
				}
				else{
					state=4;
				}
				break;
				
			case 4:
				green_blink();
				if (button == 1){
					state=5;
				}
				else{
					state=2;
				}
			break;
				
			case 5:
				all_leds_on();
			
				if (button==0){
					state=0;
				}
				break;
		}
		
	}
}

