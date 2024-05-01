#include "stm32l476xx.h"

#define LED_PIN    5
#define LED_PORT   GPIOA

void enable_HSI(){
    RCC->CR |= RCC_CR_HSION;
    while((RCC->CR & RCC_CR_HSIRDY) == 0) {}
    RCC->CFGR &= ~RCC_CFGR_SW;
    RCC->CFGR |= RCC_CFGR_SW_HSI;
    while((RCC->CFGR & RCC_CFGR_SWS) != RCC_CFGR_SWS_HSI) {}
}

void LED_Pin_Init(){
    RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;
    LED_PORT->MODER &= ~(0x03 << (2*LED_PIN));
    LED_PORT->MODER |= 0x01 << (2*LED_PIN); // Output mode
}

void TIM2_CH1_Init(){
    RCC->APB1ENR1 |= RCC_APB1ENR1_TIM2EN;
    
    TIM2->PSC = 19999; // Prescaler to get 1ms resolution (16MHz / 20000 = 800Hz)
    TIM2->ARR = 3999;  // Auto-reload to get a 2Hz frequency (800Hz / 4000 = 0.2Hz)
    
    TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;  
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_0 | TIM_CCMR1_OC1M_1; // Toggle mode
    
    TIM2->CCR1 = 2000; // Duty cycle of 50% (toggle at halfway)
    
    TIM2->CCER |= TIM_CCER_CC1E; 
    
    TIM2->CR1 |= TIM_CR1_CEN;
}

int main(void){
    enable_HSI();
    LED_Pin_Init();
    TIM2_CH1_Init();
    
    while(1);
}
