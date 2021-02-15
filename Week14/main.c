#include "stm32f10x.h"
#include <math.h>
#include <stdio.h>

GPIO_InitTypeDef GPIO_InitStructure;
EXTI_InitTypeDef EXTI_InitStructure;
NVIC_InitTypeDef NVIC_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
USART_InitTypeDef USART_InitStructure;
ADC_InitTypeDef ADC_InitStructure;

///////////////////////////////////////////////////////////////////////////////////////////////
void configureGPIO(void);
void configureEXTI(void);
void configureNVIC(void);
void configureTIM3(void);
void configureUART1(void);
void configureADC1(void);

////////////////////////////////////////My_variables////////////////////////////////////////////

float t; //Time to adjust UART frequency to publish variable in terminal(5Hz adjusted)
int button_situation; // It changes the step input situation when the button is pressed 
// Step size to solve ODE 
volatile double stepSize = 0.01; 
// System input (step) to be sent if exti triggered 
volatile double inputToSystem = 0.0; 
// System parameters 
volatile double outputToPC;

double sqrt(double arg);
double y1;
double y2;
double y3;
double K,wn,zeta; //Parameters of second order diff.eqn
double a1,a2,a3,b2,b3; //Parameters of third order diff.eqn
char output[20];

double u; //Step_input
double control_function_output; //error
int real_output;// output in bits

///////////////Comparison values for videos///////////////////////////////////////
double real_system_voltage;
double hardware_in_loop_value;

//////////////////////////////main////////////////////////////////////////////////
int main(void){
	/*
	//Gs Step Response parameters 
	wn=sqrt(1/2.2);
	K=1;
	zeta=(2.1/2.2)/wn;
	*/
	/*
	//Gs with P control Step Response parameters 
	wn=sqrt(3.5/2.2);
	K=2.5/2.2/wn/wn;
	zeta=(2.1/2.2)/wn;
	*/
	
	/*
	//Gs with PI control Step Response parameters
	a1=4.2/2.2;
	a2=3.8/2.2;
	a3=0.8/2.2;
	b2=2.8/2.2;
	b3=0.8/2.2;
	*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO ,ENABLE );
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE );
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_ADCCLKConfig(RCC_PCLK2_Div6); //Set ADC_clock(Maximum 14MHz | 72/6=12Hz)
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);	
	
	configureGPIO();
	configureEXTI();
	configureNVIC();
	configureTIM3();
	configureUART1();
	configureADC1();
	
	while(1){
		u=(TIM3->CCR1)*(3.3/400)*button_situation; //[Bits mapped to give 1 as a voltage]
		real_output = (int)(ADC_GetConversionValue(ADC1)); //Read Vout[in bits]
		outputToPC=real_output*(3.3/4096);//Vout[voltage] //Show mapped output voltage value
		//real_system_voltage=real_output*(3.3/4096);//Vout[voltage] //Show mapped output voltage value
		sprintf(output,"%lf\r",outputToPC);
	}
}
/*
//////////////2nd Order System Step Response Functions/////////////////////////////////////////////////////

// Differential Equation y1'(t) = y2
double eq1(double y1, double y2, double u){
	return y2;
}
// Differential Equation y2'(t)= (K*wn^2*u)-(2*zeta*wn*y2)-(wn*wn*y1)
double eq2(double y2,double y1, double u){
	return (K*wn*wn*u)-(2*zeta*wn*y2)-(wn*wn*y1);
}	

double ExplicitEulerEq1(double (*f)(double,double,double), double y2, double ts, double uk){
	static double yk=0;
	double yk1=yk+ts*f(yk,y2,uk); //eq1
	yk=yk1;
	return yk1;
}

double ExplicitEulerEq2(double (*f)(double,double,double), double y1, double ts, double uk){
	static double yk=0;
	double yk1=yk+ts*f(yk,y1,uk); //eq2
	yk=yk1;
	return yk1;
}
*/
/*
///////////////3rd Order System Step Response Functions//////////////////////////////////////////////////////////

// Differential Equation y1'(t) = y2
double eq1(double y1,double y2, double u){
	return y2;
}
// Differential Equation y2'(t)= y3
double eq2(double y2,double y3, double u){
	return y3+b2*u;
}	

// Differential Equation y3'(t)= -a3*y1 -a2*y2 -a3*y3 + b2*u
double eq3(double y1,double y2,double y3, double u){
	return (-a3*y1 -a2*y2 -a1*y3 + (b3-a1*b2)*u);
}	

double ExplicitEulerEq1(double (*f)(double,double,double), double y2, double ts, double uk){
	static double yk=0; //y1_initial
	double yk1=yk+ts*f(yk,y2,uk); //eq1
	yk=yk1;
	return yk1;
}

double ExplicitEulerEq2(double (*f)(double,double,double), double y3, double ts, double uk){
	static double yk=0; //y2_initial
	double yk1=yk+ts*f(yk,y3,uk); //eq2
	yk=yk1;
	return yk1;
}

double ExplicitEulerEq3(double (*f)(double,double,double,double), double y1, double y2, double ts, double uk){
	static double yk=0; //y3_initial
	double yk1=yk+ts*f(y1,y2,yk,uk); //eq3
	yk=yk1;
	return yk1;
}
*/

//////////////////////////////////////////////PID Functions////////////////////////////////////

//volatile double Kp = 2.5, Ki = 0, Kd = 0; //P control
volatile double Kp = 2.8, Ki = 0.8, Kd = 0; //PI control

// Backward differentiation 
double Derivative(double ek, double ek_1) {
	return (ek - ek_1) / stepSize; 
}

// Trapezoidal integration 
double Integral(double ek, double ek_1) {
	return (ek + ek_1) * (stepSize / 2); 
}

// PID Controller 
double PID(double r, double y) {
	static double ek_1 = 0, integralSum = 0; 
	double outputPID; 
	double ek = r - y;
	integralSum += Integral(ek, ek_1); 
	outputPID = Kp * ek + Ki * integralSum + Kd * Derivative(ek, ek_1); 
	ek_1 = ek;
  if(button_situation==0){ 
		integralSum=0;}
	return outputPID; 
}


////////////////////////Configuration Functions///////////////////////
void configureGPIO(void)
{
	// Configure input(button)
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	//Configure EXTI line(PA0)
	GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource0);
	
	// Configue UART RX - UART module's TX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	// Configue UART TX - UART module's RX should be connected to this pin
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output PA6(PWM output so Alternate-function)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure output PA7(PWM output so Alternate-function)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// Configure input PA1(Analog Input)
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AIN;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
}
void configureEXTI(void){
	EXTI_InitStructure.EXTI_Line = EXTI_Line0;
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
}

void configureNVIC(void)
{
	NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
	
  NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x01; //External interrupts priority > timer interrupt priority
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}

void configureTIM3(void)
	{
	//Configure timer base generation
	TIM_TimeBaseStructure.TIM_Period = 399;
	TIM_TimeBaseStructure.TIM_Prescaler= 1799;
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
	
	//Configure output compare mode
	//TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Timing; //TIM3_CCMR1
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //TIM3_CCMR1
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High; //TIM3_CCER
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //TIM3_CCER
	
	TIM_OCInitStructure.TIM_Pulse = 110;	
	TIM_OC1Init(TIM3, &TIM_OCInitStructure);
	TIM_OC2Init(TIM3, &TIM_OCInitStructure);
	
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);
	//Enable timer 3
	TIM_Cmd(TIM3, ENABLE);
}

void configureUART1(void)
{
//  USART_ClockInitTypeDef USART_ClockInitStructure;
	USART_InitStructure.USART_BaudRate = 19200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
	USART_Init(USART1, &USART_InitStructure);
	
//	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
	USART_Cmd(USART1, ENABLE);
}

void configureADC1(void){
	ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStructure.ADC_NbrOfChannel = 1;
	ADC_Init(ADC1, &ADC_InitStructure);
	
	ADC_RegularChannelConfig(ADC1, ADC_Channel_1, 1, ADC_SampleTime_7Cycles5);
	ADC_Cmd(ADC1, ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	ADC_SoftwareStartConvCmd(ADC1, ENABLE);
}
	
