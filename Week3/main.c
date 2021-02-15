#include "stm32f10x.h"

GPIO_InitTypeDef GPIO_InitStructure;

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

int main(void){
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	
	//GPIO_InitStructure.GPIO_Pin=GPIO_Pin_13;
	//GPIO_InitStructure.GPIO_Speed=GPIO_Speed_2MHz;
	//GPIO_InitStructure.GPIO_Mode=GPIO_Mode_Out_PP;
	//GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	RCC->APB2ENR|= (0x1<<4); //Peripheral clock enable register
	GPIOC->CRH &= ~(0xF<<20);
	GPIOC->CRH |= (0x2<<20);
	
	delayInit();

	while(1)
	{
		//GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		GPIOC->BRR |=(1<<13);
		delayMs(500); 
		
		//GPIO_SetBits(GPIOC, GPIO_Pin_13);
		GPIOC->BSRR |=(1<<13);
		delayMs(500);
		
		
	}
}