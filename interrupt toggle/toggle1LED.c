#include "stm32l476xx.h"
#include <stdint.h>

#define EXTI_PIN 1
#define LED_PIN 5

void enable_HSI(){
    // Enable High Speed Internal Clock (HSI = 16 MHz)
    RCC->CR |= RCC_CR_HSION;

    // wait until HSI is ready
    while ((RCC->CR & RCC_CR_HSIRDY) == 0) {}

    // Select HSI as system clock source
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;  //01: HSI16 oscillator used as system clock

    // Wait till HSI is used as system clock source
    while ((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}
}

void LED_Init(void){

    // Enable the peripheral clock of GPIO Port
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
    GPIOA->MODER &= ~(3U << (2 * LED_PIN));
    GPIOA->MODER |= 1U << (2 * LED_PIN);      //  Output(01)

    // GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
    GPIOA->OSPEEDR &= ~(3U << (2 * LED_PIN));
    GPIOA->OSPEEDR |= 3U << (2 * LED_PIN);  // High speed

    // GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
    GPIOA->OTYPER &= ~(1U << LED_PIN);       // Push-pull

    // GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~(3U << (2 * LED_PIN));  // No pull-up, no pull-down
}

//-------------------------------------------------------------------------------------------
// Turn LED On
//-------------------------------------------------------------------------------------------
void LED_On(void){
    GPIOA->ODR |= (1UL << LED_PIN);
}

//-------------------------------------------------------------------------------------------
// Turn LED Off
//-------------------------------------------------------------------------------------------
void LED_Off(void){
    GPIOA->ODR &= ~(1UL << LED_PIN);
}

//-------------------------------------------------------------------------------------------
// Toggle LED
//-------------------------------------------------------------------------------------------
void LED_Toggle(void){
    GPIOA->ODR ^= (1UL << LED_PIN);
    // Introduce a short delay to speed up the toggle
    for (volatile uint32_t i = 0; i < 100000; ++i) {} // Adjust this delay as needed
}

void EXTI_Init(void){

    // GPIO Configuration
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // GPIO Mode: Input(00, reset), Output(01), AlterFunc(10), Analog(11, reset)
    GPIOA->MODER &= ~(3U << (2 * EXTI_PIN)); // Input mode

    // GPIO Pull-down: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~(3U << (2 * EXTI_PIN)); // Pull-down

    // Enable SYSCFG clock
    RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;

    // Connect EXTI line to the GPIO pin for PA1
    SYSCFG->EXTICR[0] &= ~(0x0F << (4 * EXTI_PIN)); // Clear the EXTI1 bits
    SYSCFG->EXTICR[0] |= (0x00 << (4 * EXTI_PIN)); // Set EXTI1 to connect to GPIOA

    // Enable and set EXTI line 1 interrupt to the lowest priority
    NVIC_SetPriority(EXTI1_IRQn, 0);
    NVIC_EnableIRQ(EXTI1_IRQn);

    // Enable EXTI line 1
    EXTI->IMR1 |= (1 << EXTI_PIN);

    // Trigger on rising edge
    EXTI->RTSR1 |= (1 << EXTI_PIN);
}

void EXTI1_IRQHandler(void){
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    if ((EXTI->PR1 & (1 << EXTI_PIN)) != 0){
        // Clear the EXTI pending bit
        EXTI->PR1 |= (1 << EXTI_PIN);
        LED_Toggle();
    }
}

int main(void){

    enable_HSI();
    LED_Init();
    EXTI_Init();

    while (1) {}
}
