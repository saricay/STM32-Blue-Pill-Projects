#include "stm32f10x.h"
#include "delay.h"

GPIO_InitTypeDef GPIO_InitStructure; //Peripheral Library is activated

void all_leds_on(){
		GPIO_SetBits(GPIOB, GPIO_Pin_10); //Give Output as 1 to the pin B10
		GPIO_SetBits(GPIOB, GPIO_Pin_1); //Give Output as 1 to the pin B1
		GPIO_SetBits(GPIOB, GPIO_Pin_0); //Give Output as 1 to the pin B0
	}

void all_leds_off(){
		GPIO_ResetBits(GPIOB, GPIO_Pin_10); //Reset to the output pin @B10
		GPIO_ResetBits(GPIOB, GPIO_Pin_1); //Reset to the output pin @B1
		GPIO_ResetBits(GPIOB, GPIO_Pin_0); //Reset to the output pin @B0
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
	int i;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); // PORTB activated
	define_pins(); //Pin definition function is called
	delayInit();
	state=0;
	
	while(1) 
	{
		button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12); // Read button @B12
		switch(state) //Introducing the Finite State Design
			{
			case 0:			//In case 0: all leds are off until the button is pressed
				all_leds_off();
				if (button==1){
					state=1;
				}
				break;
			
			case 1: //In case 1: open leds are commanded to turn off since it is going to start blinking
				all_leds_on();
				if (button==0){
					state=2;
				}
				all_leds_off();
				break;
				
			case 2: //In case 2: it started blinking from red led. If button is pressed then cut the blinking(state 5), else pass to state 3 which is yellow on, others off
				i=0;
				while (i<50) //50x10 Ms= 500Ms open time for red led in a period 
				{ 
					GPIO_SetBits(GPIOB, GPIO_Pin_10); //turn on red led
					delayMs(200);
					button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12); 
					GPIO_ResetBits(GPIOB, GPIO_Pin_10);  //turn off red led
					if (button == 1){
						state=5;
						i=50000;
					}
					i=i+1;
					
				}	
				if (state != 5){
					state=3;}
				break;
			
			case 3: //In case 3: yellow led is on. If button is pressed then cut the blinking(state 5), else pass to state 4 which is green on, others off
				i=0;
				while (i<50){
					GPIO_SetBits(GPIOB, GPIO_Pin_1); //turn on yellow led
					delayMs(40);	
					button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12); 
					GPIO_ResetBits(GPIOB, GPIO_Pin_1); //turn off yellow led
					if (button == 1){
						state=5;
						i=50000;
					}
					i=i+1;
				}
				if (state != 5){
					state=4;}
				break;
				
			case 4: //In case 4: green led is on. If button is pressed then cut the blinking(state 5), else pass to state 2 which is red on, others off
				i=0;
				while (i<50){
					GPIO_SetBits(GPIOB, GPIO_Pin_0); //turn on green led
					delayMs(200);
					button= GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12);
					GPIO_ResetBits(GPIOB, GPIO_Pin_0); //turn off green led
					if (button == 1){
						state=5;
						i=50000;
					}
					i=i+1;
				}
				if (state != 5){
					state=2;
				}
			break;

			case 5: //In case 5: When a button is hold pressed in blinking status, it comes to state 5 and all leds are on. 
						//When it released, they all turn off, go to state 0 
				all_leds_on();
			
				if (button==0){
					state=0;
				}
				break;
		}
	}
}
