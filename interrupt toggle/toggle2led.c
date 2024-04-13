#include "stm32l476xx.h"
#include <stdint.h>

#define EXTI_PIN 1
#define EXTI_PIN_2 7
#define LED_PIN 5
#define LED_PIN_2 6

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
    // Enable the peripheral clock of GPIO Port A
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

void LED_Init_2(void){
    // Enable the peripheral clock of GPIO Port A
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;

    // GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
    GPIOA->MODER &= ~(3U << (2 * LED_PIN_2));
    GPIOA->MODER |= 1U << (2 * LED_PIN_2);      //  Output(01)

    // GPIO Speed: Low speed (00), Medium speed (01), Fast speed (10), High speed (11)
    GPIOA->OSPEEDR &= ~(3U << (2 * LED_PIN_2));
    GPIOA->OSPEEDR |= 3U << (2 * LED_PIN_2);  // High speed

    // GPIO Output Type: Output push-pull (0, reset), Output open drain (1)
    GPIOA->OTYPER &= ~(1U << LED_PIN_2);       // Push-pull

    // GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~(3U << (2 * LED_PIN_2));  // No pull-up, no pull-down
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
    GPIOA->MODER &= ~((3U << (2 * EXTI_PIN)) | (3U << (2 * EXTI_PIN_2))); // Input mode

    // GPIO Pull-down: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
    GPIOA->PUPDR &= ~((3U << (2 * EXTI_PIN)) | (3U << (2 * EXTI_PIN_2))); // Pull-down

    // Connect EXTI line to the GPIO pin for PA1 and PA7
    SYSCFG->EXTICR[0] &= ~((0x0F << (4 * EXTI_PIN)) | (0x0F << (4 * EXTI_PIN_2))); // Clear EXTI bits
    SYSCFG->EXTICR[0] |= (0x00 << (4 * EXTI_PIN)) | (0x00 << (4 * EXTI_PIN_2)); // Set EXTI to connect to GPIOA

    // Enable and set EXTI line 1 and 7 interrupts to the lowest priority
    NVIC_SetPriority(EXTI1_IRQn, 0);
    NVIC_SetPriority(EXTI9_5_IRQn, 0);
    NVIC_EnableIRQ(EXTI1_IRQn);
    NVIC_EnableIRQ(EXTI9_5_IRQn);

    // Enable EXTI line 1 and 7
    EXTI->IMR1 |= (1 << EXTI_PIN);
    EXTI->IMR1 |= (1 << EXTI_PIN_2);

    // Trigger on rising edge for EXTI1 and EXTI7
    EXTI->RTSR1 |= (1 << EXTI_PIN);
    EXTI->RTSR1 |= (1 << EXTI_PIN_2);
}

void EXTI1_IRQHandler(void){
    NVIC_ClearPendingIRQ(EXTI1_IRQn);
    if ((EXTI->PR1 & (1 << EXTI_PIN)) != 0){
        // Clear the EXTI pending bit
        EXTI->PR1 |= (1 << EXTI_PIN);
        LED_Toggle();
    }
}

void EXTI9_5_IRQHandler(void){
    NVIC_ClearPendingIRQ(EXTI9_5_IRQn);
    if ((EXTI->PR1 & (1 << EXTI_PIN_2)) != 0){
        // Clear the EXTI pending bit
        EXTI->PR1 |= (1 << EXTI_PIN_2);
        // Toggle the second LED
        GPIOA->ODR ^= (1UL << LED_PIN_2);
        // Introduce a short delay
        for (volatile uint32_t i = 0; i < 100000; ++i) {} // Adjust as needed
    }
}

int main(void){
    enable_HSI();
    LED_Init();
    LED_Init_2(); // Initialize the second LED
    EXTI_Init();

    while (1) {}
}
