#include "stm32f10x.h" 

#define button1 GPIO_Pin_0  //A0
#define button2 GPIO_Pin_3 //B3
#define led_port GPIOA
#define red GPIO_Pin_1
#define yellow GPIO_Pin_2
#define green GPIO_Pin_3

void CONFIG_GPIO(void);
void CONFIG_UART1(void);
void CONFIG_ADC1(void);
void UART_Transmit(char *string);
int READ_ADC(void);
