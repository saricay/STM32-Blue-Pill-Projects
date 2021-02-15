#include "delay.h"
 
static volatile uint32_t ticks;
//extern int buttonPressed;

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
	{
		delayUs(1000);
	}	
} 
