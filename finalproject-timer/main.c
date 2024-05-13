#include "stm32l476xx.h"

// TOGGLE THE led ATTACHED TO gpio pa% EVERY 5 SECONDS 
#define LED_PIN    5
#define LED_PORT   GPIOA
#define BUTTON_PIN 6

// User HSI (high-speed internal) as the processor clock
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

void LED_Pin_Init(){
	  RCC->AHB2ENR 		|= RCC_AHB2ENR_GPIOAEN;             // Enable GPIOA clock
	
	  // Set mode as Alternative Function 1
		LED_PORT->MODER  	&= ~(0x03 << (2*LED_PIN));   			// Clear bits
		LED_PORT->MODER  	|=   0x02 << (2*LED_PIN);      		// Input(00), Output(01), AlterFunc(10), Analog(11)
	
		LED_PORT->AFR[0] 	&= ~(0xF << (4*LED_PIN));         // 	AF 1 = TIM2_CH1
		LED_PORT->AFR[0] 	|=   0x1 << (4*LED_PIN);          // 	AF 1 = TIM2_CH1
	
		//Set I/O output speed value as very high speed
		LED_PORT->OSPEEDR  &= ~(0x03<<(2*LED_PIN)); 				// Speed mask
		//LED_PORT->OSPEEDR  |=   0x03<<(2*LED_PIN); 					// Very high speed
		//Set I/O as no pull-up pull-down 
		LED_PORT->PUPDR    &= ~(0x03<<(2*LED_PIN));    			// No PUPD(00, reset), Pullup(01), Pulldown(10), Reserved (11)
		//Set I/O as push pull 
	  //LED_PORT->OTYPER   &=  ~(1<<LED_PIN) ;           	// Push-Pull(0, reset), Open-Drain(1)
}


void configure_button_pin(){
	// 
  // Enable the clock to GPIO Port A
  RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   
		
	// GPIO Mode: Input(00), Output(01), AlterFunc(10), Analog(11, reset)
	GPIOA->MODER &= ~(3UL<<(2*BUTTON_PIN));  // Input(00)
	
	// GPIO Push-Pull: No pull-up, pull-down (00), Pull-up (01), Pull-down (10), Reserved (11)
	GPIOA->PUPDR  &= ~(3<<(2*BUTTON_PIN));  // No pull-up, no pull-down
		GPIOA->PUPDR |= (1<<(2*BUTTON_PIN));  // No pull-up, no pull-down
}

void TIM2_CH1_Init(uint32_t initalDutyCycle){
	
		// Enable the timer clock
    RCC->APB1ENR1 		|= RCC_APB1ENR1_TIM2EN;                  // Enable TIMER clock

		// Counting direction: 0 = up-counting, 1 = down-counting
		TIM2->CR1 &= ~TIM_CR1_DIR;  
		
    TIM2->PSC = 300;       // Prescaler = 
    TIM2->ARR = 999;   // Auto-reload: Upcouting (0..ARR), Downcouting (ARR..0)
	  TIM2->CCR1  = initalDutyCycle;         // Output Compare Register for channel 1, 
    // select PWM mode 1 output on channel 1	(OC1M=110)
		TIM2->CCMR1 &= ~TIM_CCMR1_OC1M;  // Clear ouput compare mode bits for channel 1
    TIM2->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // set mode to 110  -- upcounts and is active when count < target, else inactive
    
	// enable preload of CCR1, i.e., update synchrounsly.
	  TIM2->CCMR1 |= TIM_CCMR1_OC1PE;                     
		
    // Enable output for ch1
		TIM2->CCER |= TIM_CCER_CC1E; // map OC to pin TIM2_CH1                       
    
		TIM2->CR1  |= TIM_CR1_CEN; // Enable counter
}


int main(void){
	int i;
	enable_HSI();
	LED_Pin_Init();
	configure_button_pin();
	TIM2_CH1_Init(200);
	
  // Dead loop & program hangs here
	while (1) {
        if (!(GPIOA->IDR & (1 << BUTTON_PIN))) { // Check if the button is pressed
						TIM2_CH1_Init(130);

            // Wait until the button is released
            while (!(GPIOA->IDR & (1 << BUTTON_PIN))) {}
        } else {
            						TIM2_CH1_Init(900);
   
        }
    }
}
