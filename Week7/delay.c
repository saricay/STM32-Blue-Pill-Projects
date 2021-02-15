#include "delay.h"
#include "stm32f10x_it.h"
static volatile uint32_t ticks;

void delayInit() 
{ 
  SystemCoreClockUpdate(); 
	SysTick_Config(SystemCoreClock / 1000000); // Overflow every 1 us 
} 

void delayDecrement(void)
{
	if (ticks != 0x00) 
 		ticks--;
}

void delayUs(uint32_t nTime) 
{ 
 	ticks = nTime; 
 	while(ticks); 
}

void delayMs(uint32_t nTime) 
{
	while(nTime--)  
		
 		delayUs(1000); 
} 

static uint8_t durum2=1;
//Initially durum2=1, durum1=0. If button is pressed, durum1 changes as 1. if(1&&1) gives change the value of durum2 as 0. When button is pressed again,  
void delayMs_2(uint32_t nTime) 																			//durum1 changes the its value as 0. (0' || 0') gives again 1.											
{																																		//Then if(1&&1) changes the value of durum2 again as 1 => Initial condition
	while(nTime--)  {
			if ( ( durum2 && durum_1() ) ||  (!durum2 && !durum_1() ) ){ //If both variables equal at the same time, 																									
				nTime=0;																									 //rest time is resetted. Unless the 
				durum2^=1;																								 //the variables are the same, button is unable 																													 
			}																														 //to change the value of rest of the time
			else delayUs(1000);																					 //if button is not sensed, continue orderly blinking.
	}
} 

uint8_t durum_2(void){																						 //
return durum2;
}
