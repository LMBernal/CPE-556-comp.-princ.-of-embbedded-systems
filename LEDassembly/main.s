; modified from the original version to blink the LED and not to use constants
******************** (C) Yifeng ZHU *******************************************
; @file    main.s
; @author  Yifeng Zhu
; @date    May-17-2015
; @note
;           This code is for the book "Embedded Systems with ARM Cortex-M 
;           Microcontrollers in Assembly Language and C, Yifeng Zhu, 
;           ISBN-13: 978-0982692639, ISBN-10: 0982692633
; @attension
;           This code is provided for education purpose. The author shall not be 
;           held liable for any direct, indirect or consequential damages, for any 
;           reason whatever. More information can be found from book website: 
;           http:;www.eece.maine.edu/~zhu/book
;*******************************************************************************


	;INCLUDE core_cm4_constants.s		; Load Constant Definitions
	;INCLUDE stm32l476xx_constants.s      

; Green LED <--> PA.5
LED_PIN	EQU	5
GPIO_MODER      EQU  0x00      ; GPIO port mode register,               Address offset: 0x00
GPIO_OTYPER     EQU  0x04      ; GPIO port output type register,        Address offset: 0x04
GPIO_OSPEEDR    EQU  0x08      ; GPIO port output speed register,       Address offset: 0x08
GPIO_PUPDR      EQU  0x0C      ; GPIO port pull-up/pull-down register,  Address offset: 0x0C
GPIO_IDR        EQU  0x10      ; GPIO port input data register,         Address offset: 0x10
GPIO_ODR        EQU  0x14      ; GPIO port output data register,        Address offset: 0x14
GPIO_BSRR       EQU  0x18      ; GPIO port bit set/reset  register,     Address offset: 0x18
GPIO_LCKR       EQU  0x1C      ; GPIO port configuration lock register, Address offset: 0x1C
GPIO_AFR0       EQU  0x20      ; GPIO alternate function registers,     Address offset: 0x20-0x24
GPIO_AFR1       EQU  0x24      ; GPIO alternate function registers,     Address offset: 0x20-0x24
GPIO_BRR        EQU  0x28      ; GPIO Bit Reset register,               Address offset: 0x28
GPIO_ASCR       EQU  0x2C      ; GPIO analog switch control register,   Address offset: 0x2C

PERIPH_BASE   EQU (0x40000000) ; Peripheral base address
AHB1PERIPH_BASE       EQU (PERIPH_BASE + 0x00020000)
RCC_BASE      EQU (AHB1PERIPH_BASE + 0x1000)	
AHB2PERIPH_BASE       EQU (PERIPH_BASE + 0x08000000)
RCC_AHB2ENR_GPIOAEN         EQU (0x00000001)
RCC_AHB2ENR     EQU  0x4C  ; RCC AHB2 peripheral clock enable register,                               Address offset: 0x4C
GPIOA_BASE    EQU (AHB2PERIPH_BASE + 0x0000)
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
		
    ; Enable the clock to GPIO Port A	
	    LDR r0, =RCC_BASE
	    LDR r1, [r0, #RCC_AHB2ENR]
	    ORR r1, r1, #RCC_AHB2ENR_GPIOAEN
	    STR r1, [r0, #RCC_AHB2ENR]
	
	; MODE: 00: Input mode, 01: General purpose output mode
    ;       10: Alternate function mode, 11: Analog mode (reset state)
	    LDR r0, =GPIOA_BASE
	    LDR r1, [r0, #GPIO_MODER]
	    BIC r1, r1, #(3<<(2*LED_PIN))
	    ORR r1, r1, #(1<<(2*LED_PIN))    ;output (01)
	    STR r1, [r0, #GPIO_MODER]

        ; OUTPUT TYPE: 0: Push-Pull, 1: Open drain 
	    LDR r1, [r0, #GPIO_OTYPER]
	    AND r1, r1,  #~(1<<LED_PIN)
	    STR r1, [r0, #GPIO_OTYPER]

	    LDR r1, [r0, #GPIO_ODR]
repeat	ORR r1, r1, #(1<<LED_PIN)        ;turn LED on
	    STR r1, [r0, #GPIO_ODR]
   
        LDR r2, = 0x25000
Delay   SUBS r2, r2, #1
	    			
	    BGT  Delay
		LDR r1, [r0, #GPIO_ODR]
	    BIC r1, r1, #(1<<LED_PIN)        ;turn LED OFF
	    STR r1, [r0, #GPIO_ODR]
        LDR r2, = 0x25000
Delay2 SUBS r2, r2, #1
	   				
	    BGT  Delay2
  
 	    B 		repeat   		; dead loop & program hangs here

	;ENDP
					

	   END
