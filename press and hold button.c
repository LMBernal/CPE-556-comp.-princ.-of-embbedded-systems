#include "stm32l476xx.h"

#define LED_PIN    5
#define BUTTON_PIN 1

void enable_HSI() {
    // Enable High Speed Internal Clock (HSI = 16 MHz)
    RCC->CR |= RCC_CR_HSION;
    
    // Wait until HSI is ready
    while (!(RCC->CR & RCC_CR_HSIRDY));
    
    // Select HSI as system clock source 
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;  // HSI16 oscillator used as system clock

    // Wait till HSI is used as system clock source 
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI);
}

void configure_LED_pin() {
    // Enable the clock to GPIO Port A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   

    // GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
    GPIOA->MODER &= ~(3UL << (2 * LED_PIN));
    GPIOA->MODER |= 1UL << (2 * LED_PIN);      // Output(01)

    // GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
    GPIOA->OSPEEDR &= ~(3 << (2 * LED_PIN)); // Low speed (00)

    // GPIO Output Type: Output push-pull (0, reset), Output open drain (1) 
    GPIOA->OTYPER &= ~(1 << LED_PIN);      // Push-pull

    // GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~(3 << (2 * LED_PIN));  // No pull-up, no pull-down
}

void configure_button_pin() {
    // Enable the clock to GPIO Port C
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOCEN;   

    // GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
    GPIOC->MODER &= ~(3UL << (2 * BUTTON_PIN));  // Input(00)

    // Enable internal pull-down resistor
    GPIOC->PUPDR |= (2 << (2 * BUTTON_PIN));  // Pull-down (10)
}

void turn_on_LED() {
    GPIOA->ODR |= 1 << LED_PIN;
}

void turn_off_LED() {
    GPIOA->ODR &= ~(1 << LED_PIN);
}

void toggle_LED() {
    GPIOA->ODR ^= (1 << LED_PIN);
}

int main(void) {
    int i;
    enable_HSI();
    configure_LED_pin();
    configure_button_pin();
    turn_off_LED();
    
    while (1) {
        // Check if the button is pressed
        if (GPIOC->IDR & (1 << BUTTON_PIN)) {
            // Button is pressed, start blinking LED
            while (GPIOC->IDR & (1 << BUTTON_PIN)) {
                for (i = 0; i < 1000000; i++); // simple delay
                toggle_LED();
            }
        } else {
            // Button is released, turn off LED
            turn_off_LED();
        }
    }
}
