#include "EXTI.h"
#include "LED.h"

// Push button = PC 13 
#define EXTI_PIN 13

void EXTI_Init(void){
	// GPIO Configuration
	RCC->AHB2ENR |=   RCC_AHB2ENR_GPIOCEN;
	
	// GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOC->MODER &= ~(3U<<(2*EXTI_PIN)); //inputs

	// GPIO PUDD: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)	
	GPIOC->PUPDR &= ~(3U<<(2*EXTI_PIN)); // no pull-up, no pull down
	// GPIOC->PUPDR |=  (1U<<(2*EXTI_PIN));
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
	
	// EXIT Interrupt Enable
	NVIC_EnableIRQ(EXTI15_10_IRQn); 
  NVIC_SetPriority(EXTI15_10_IRQn, 0);  
	
	// Connect External Line to the GPIO
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	SYSCFG->EXTICR[3] &= ~SYSCFG_EXTICR4_EXTI13;     // SYSCFG external interrupt configuration registers
	SYSCFG->EXTICR[3] |=  SYSCFG_EXTICR4_EXTI13_PC; 
	
	// Interrupt Mask Register (IMR)
	EXTI->IMR1 |= EXTI_IMR1_IM13;     // 0 = marked, 1 = not masked (i.e., enabled)
	
	// Software interrupt event register
	// EXTI->SWIER1 |= EXTI_SWIER1_SWI0;
	
	// Rising trigger selection register (RTSR)
	// EXTI->RTSR1 |= EXTI_RTSR1_RT13;  // 0 = disabled, 1 = enabled
	
	// Falling trigger selection register (FTSR)
	EXTI->FTSR1 |= EXTI_FTSR1_FT13;  // 0 = disabled, 1 = enabled
}

void EXTI15_10_IRQHandler(void) {  
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	// PR: Pending register
	if ((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13) {
		// cleared by writing a 1 to this bit
		EXTI->PR1 |= EXTI_PR1_PIF13;
		LED_Toggle();
	}
}
