#include "main.h"
#include "Timer_Delay.h"
#include "DWT_Delay.h"
#define GPIOAEN				(1U<<0)
#define	PIN5_H				(1U<<5)
#define PIN5_L				(1U<<21)

uint8_t Response = 0;
uint8_t Temperature_H;
uint8_t Temperature_L;
uint8_t Humidity_H;
uint8_t Humidity_L;
uint8_t Check_Sum;

uint16_t Temp, Humid;

float Temperature = 0;
float Humidity = 0;

void DHT_Init(void){
	RCC->AHB1ENR |= GPIOAEN;
}

void DHT_Start(void){
	GPIOA->MODER |= (1<<10);
	GPIOA->MODER &=~(1<<11);
	GPIOA->ODR &=~ PIN5_H;
	delay_ms(18);
	GPIOA->ODR |= PIN5_H;
	delay_us(20);
	GPIOA->MODER &=~(1<<10);
	GPIOA->MODER &=~(1<<11);
	GPIOA->PUPDR |= (1<<10);
	GPIOA->PUPDR &=~(1<<11);
}

uint8_t check_Response(void){
	delay_us(40);
	if(!(GPIOA->IDR & PIN5_H)){
		delay_us(80);
		if(GPIOA->IDR & PIN5_H){
			Response = 1;
		}
		else{
			Response = -1;
		}
	}
	while((GPIOA->IDR & PIN5_H));
	return Response;
}

uint8_t DHT_Read(void){
	uint8_t data;
	for(int i = 0;i<8;i++){
		while(!(GPIOA->IDR & PIN5_H));
		delay_us(40);
		if(!(GPIOA->IDR & PIN5_H)){
			data &=~(1<<(7-i));
		}
		else{
			data |= (1<<(7-i));
		}
		while(GPIOA->IDR & PIN5_H);
	}
	return data;
}

int main(void){
  TimerDelay_Init();
  DHT_Init();

  while (1){
	  DHT_Start();
	  Response = check_Response();
	  Humidity_H = DHT_Read();
	  Humidity_L = DHT_Read();
	  Temperature_H = DHT_Read();
	  Temperature_L = DHT_Read();
	  Check_Sum = DHT_Read();
	  Temperature = (float) ((Temperature_H));
	  Humidity = (float) ((Humidity_H));
	  delay_ms(3000);
  }
}

void Error_Handler(void)
{
  __disable_irq();
  while (1)
  {
  }
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line){}
#endif /* USE_FULL_ASSERT */
