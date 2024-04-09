#include "stm32l476xx.h"
#include "SysClock.h"
#include "EXTI.h"
#include "LED.h"

volatile int32_t i;

// Push joystick center and toggle RED LED
int main(void){
	i = -1;
	System_Clock_Init(); // Switch System Clock = 80 MHz
	LED_Init();
	EXTI_Init();
	while(1);
}

