
	INCLUDE core_cm4_constants.s		; Load Constant Definitions
	INCLUDE stm32l476xx_constants.s      

; Green LED <--> PA.5
; User Button <--> PC.13
LED_PIN	EQU	5
USR_BUTTON EQU 13
	
	AREA    main, CODE, READONLY
	EXPORT	__main				; make __main visible to linker
	ENTRY			
				
__main	PROC
		
    ; Enable the clock to GPIO Port A and C.	
	LDR r0, =RCC_BASE
	LDR r1, [r0, #RCC_AHB2ENR]
	ORR r1, r1,  #RCC_AHB2ENR_GPIOAEN
	ORR r1, r1,  #RCC_AHB2ENR_GPIOCEN
	STR r1, [r0, #RCC_AHB2ENR]
	
	; GPIOA MODE: 00: Input mode, 01: General purpose output mode
    ;             10: Alternate function mode, 11: Analog mode (reset state)
	LDR r0, =GPIOA_BASE
	LDR r1, [r0, #GPIO_MODER]
	AND r1, r1,  #~(3<<(2*LED_PIN)) ; bits 10,11 are set to 00
	ORR r1, r1,  #(1<<(2*LED_PIN)) ; bits 10,11 are set to 01
	STR r1, [r0, #GPIO_MODER]
	
	; GPIOC MODE: 00: Input mode, 01: General purpose output mode
    ;             10: Alternate function mode, 11: Analog mode (reset state)
	LDR r2, =GPIOC_BASE
	LDR r3, [r2, #GPIO_MODER]
	AND r3, r3,  #~(3<<(2*USR_BUTTON)); bits 10,11 are set to 00 as an input
	STR r3, [r2, #GPIO_MODER]
	
	; OUTPUT TYPE: 0: Push-Pull, 1: Open drain 
	LDR r1, [r0, #GPIO_OTYPER]
	AND r1, r1,  #~(1<<LED_PIN)
	STR r1, [r0, #GPIO_OTYPER]
	
	; LED PULL UP PULL DOWN TYPE: 00: No pull up no pull down, 01: Pull up
	;                             10: Pull Down, 11: Reserved
	LDR r1, [r0, #GPIO_PUPDR]
	AND r1, r1,  #~(3<<(2*LED_PIN))
	STR r1, [r0, #GPIO_PUPDR]
	
	; BUTTON PULL UP PULL DOWN TYPE: 00: No pull up no pull down, 01: Pull up
	;                                10: Pull Down, 11: Reserved
	LDR r3, [r2, #GPIO_PUPDR]
	AND r3, r3,  #~(3<<(2*USR_BUTTON))
	ORR r3, r3,  #(1<<(2*USR_BUTTON)) ; bits 26,27 are set to 01 Pull up
	STR r3, [r2, #GPIO_PUPDR]
	
	
progloop ;program runs in an infinite loop

	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_IDR]
	TST r1,  #(1<<USR_BUTTON)
	;AND r2, r1,  #(1<<USR_BUTTON) 	; select bit 13 in IDR register for USR_BUTTON
	;CMP r2, #(1<<USR_BUTTON) 		; check if button is pressed
	BNE progloop 						; restart loop if button is not pressed
		
debounce
	LDR r0, =GPIOC_BASE
	LDR r1, [r0, #GPIO_IDR] 		; load contents of IDR to r1
	;AND r2, r1,  #(1<<USR_BUTTON) 	; select bit 13 of IDR
	;CMP r2, #0 						; check if button has been released
	TST r1,  #(1<<USR_BUTTON)
	BEQ debounce
	
turnledoff
	CMP r5, #1
	BNE turnledon 					; switch to turnledonbranch if led is off
	LDR r3, =GPIOA_BASE
	;LDR r4, [r3, #(GPIO_BSRR+16)] 	; using BSRR instead of ODR as something cool
	;EOR r4, r4,  #(1<<LED_PIN)
	MOV r4, #(1<<(LED_PIN+16))
	STR r4, [r3, #(GPIO_BSRR)]
	LDR r5, =0 						; keep track of current led status
	B progloop
		
turnledon
	LDR r3, =GPIOA_BASE
	;LDR r4, [r3, #(GPIO_BSRR)] 		; using BSRR instead of ODR as something cool
	;EOR r4, r4,  #(1<<LED_PIN)
	MOV r4, #(1<<(LED_PIN))
	STR r4, [r3, #(GPIO_BSRR)]
	LDR r5, =1 						; keep track of led status
	;B endif
;endif
	B progloop

	END
