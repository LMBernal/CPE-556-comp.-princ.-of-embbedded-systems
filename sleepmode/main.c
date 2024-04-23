#include "stm32l476xx.h"
#define EXTI_PIN 13
#define LED_PIN 5


void enable_HSI(){
	// Enable High Speed Internal Clock (HSI = 16 MHz)
  RCC->CR |= ((uint32_t)RCC_CR_HSION);
	
  // wait until HSI is ready
  while ( (RCC->CR & (uint32_t) RCC_CR_HSIRDY) == 0 ) {;}
	
  // Select HSI as system clock source 
  RCC->CFGR &= (uint32_t)((uint32_t)~(RCC_CFGR_SW));
  RCC->CFGR |= (uint32_t)RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

  // Wait till HSI is used as system clock source 
  while ((RCC->CFGR & (uint32_t)RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI  ) {;} 
}


void LED_Init(void){
	
	// Enable the peripheral clock of GPIO Port	
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;	

	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3U<<(2*LED_PIN));  
	GPIOA->MODER |= 1U<<(2*LED_PIN);      //  Output(01)
	
	// GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
	GPIOA->OSPEEDR &= ~(3U<<(2*LED_PIN));
	GPIOA->OSPEEDR |=   3U<<(2*LED_PIN);  // High speed
	
	// GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
	GPIOA->OTYPER &= ~(1U<<LED_PIN);       // Push-pull
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR   &= ~(3U<<(2*LED_PIN));  // No pull-up, no pull-down
	
}

//-------------------------------------------------------------------------------------------
// Turn LED On
//-------------------------------------------------------------------------------------------
void LED_On(void){
	GPIOA->ODR |= (1UL<<LED_PIN);
}

//-------------------------------------------------------------------------------------------
// Turn LED Off
//-------------------------------------------------------------------------------------------
void LED_Off(void){
	GPIOA->ODR &= ~(1UL<<LED_PIN);
}

//-------------------------------------------------------------------------------------------
// Toggle LED 
//-------------------------------------------------------------------------------------------
void LED_Toggle(void){
	GPIOA->ODR ^= (1UL<<LED_PIN);
}

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
	int j,i;
	NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
	// PR: Pending register
	if ((EXTI->PR1 & EXTI_PR1_PIF13) == EXTI_PR1_PIF13) {
		// cleared by writing a 1 to this bit
		EXTI->PR1 |= EXTI_PR1_PIF13;
		for (i=0; i<10; i++){
			for(j=0; j<100000; j++);
		  LED_Toggle();
		}
	}
}

// Push joystick center and toggle RED LED
int main(void){

	//i = -1;
	enable_HSI();
	LED_Init();
	EXTI_Init();
	while (1){
	      SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk; //SCB: System Contol Block, SCR: System Control Regiter
	      //SCB->SCR |= SCB_SCR_SLEEPONEXIT_Msk; // Sleep on exit
	      SCB->SCR &= ~SCB_SCR_SLEEPONEXIT_Msk; // 0, do not sleep on exit, 1, sleep on exit
		__WFI();
	    LED_On();
	}
}

