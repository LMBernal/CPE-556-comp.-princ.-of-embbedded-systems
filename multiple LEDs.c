for 4
#include "stm32l476xx.h"

// PA.5  <--> Green LED
// PA.6  <--> Blue LED
// PA.7  <--> Red LED
#define GREEN_LED_PIN    5 //red
#define BLUE_LED_PIN     6 // yellow
#define RED_LED_PIN      7 //blue

// User HSI (high-speed internal) as the processor clock
void enable_HSI(){
    // Enable High Speed Internal Clock (HSI = 16 MHz)
    RCC->CR |= RCC_CR_HSION;

    // Wait until HSI is ready
    while (!(RCC->CR & RCC_CR_HSIRDY));
    
    // Select HSI as system clock source 
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

    // Wait till HSI is used as system clock source 
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI); 
}

void configure_LED_pins(){
    // Enable the clock to GPIO Port A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // Configure Green LED pin
    GPIOA->MODER &= ~(3UL << (2 * GREEN_LED_PIN));
    GPIOA->MODER |= 1UL << (2 * GREEN_LED_PIN);   // Output mode
    
    // Configure Blue LED pin
    GPIOA->MODER &= ~(3UL << (2 * BLUE_LED_PIN));
    GPIOA->MODER |= 1UL << (2 * BLUE_LED_PIN);    // Output mode
    
    // Configure Red LED pin
    GPIOA->MODER &= ~(3UL << (2 * RED_LED_PIN));
    GPIOA->MODER |= 1UL << (2 * RED_LED_PIN);     // Output mode
}

void turn_on_LED(uint32_t pin){
    GPIOA->ODR |= (1 << pin);
}

void turn_off_LED(uint32_t pin){
    GPIOA->ODR &= ~(1 << pin);
}

void toggle_LED(uint32_t pin){
    GPIOA->ODR ^= (1 << pin);
}

void delay(uint32_t count){
	int i;
    for( i = 0; i < count; i++);
}

int main(void){
    enable_HSI();
    configure_LED_pins();
		int i;
	turn_on_LED(GREEN_LED_PIN);
	turn_on_LED(RED_LED_PIN);
	turn_on_LED(BLUE_LED_PIN);

    while(1){
      turn_on_LED(GREEN_LED_PIN);
			delay(500000);
			turn_off_LED(GREEN_LED_PIN);
			
			/////////
			
			turn_on_LED(RED_LED_PIN);
			delay(1000000);
			
			turn_off_LED(RED_LED_PIN);
			
			//////////
			
			turn_on_LED(BLUE_LED_PIN);
			delay(2000000);
			
			turn_off_LED(BLUE_LED_PIN);
			
		
	
    }
}
