;****************** main.s ***************
; Program written by: Sanjay Gorur and Keshav Narasimhan
; Date Created: 2/4/2017
; Last Modified: 2/18/2021
; Brief description of the program
;   The LED toggles at 2 Hz and a varying duty-cycle
; Hardware connections (External: One button and one LED)
;  PE1 is Button input  (1 means pressed, 0 means not pressed)
;  PE2 is LED output (1 activates external LED on protoboard)
;  PF4 is builtin button SW1 on Launchpad (Internal) 
;        Negative Logic (0 means pressed, 1 means not pressed)
; Overall functionality of this system is to operate like this
;   1) Make PE2 an output and make PE1 and PF4 inputs.
;   2) The system starts with the the LED toggling at 2Hz,
;      which is 2 times per second with a duty-cycle of 30%.
;      Therefore, the LED is ON for 150ms and off for 350 ms.
;   3) When the button (PE1) is pressed-and-released increase
;      the duty cycle by 20% (modulo 100%). Therefore for each
;      press-and-release the duty cycle changes from 30% to 70% to 70%
;      to 90% to 10% to 30% so on
;   4) Implement a "breathing LED" when SW1 (PF4) on the Launchpad is pressed:
;      a) Be creative and play around with what "breathing" means.
;         An example of "breathing" is most computers power LED in sleep mode
;         (e.g., https://www.youtube.com/watch?v=ZT6siXyIjvQ).
;      b) When (PF4) is released while in breathing mode, resume blinking at 2Hz.
;         The duty cycle can either match the most recent duty-
;         cycle or reset to 30%.
;      TIP: debugging the breathing LED algorithm using the real board.
; PortE device registers
GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_AFSEL_R EQU 0x40024420
GPIO_PORTE_DEN_R   EQU 0x4002451C
; PortF device registers
GPIO_PORTF_DATA_R  EQU 0x400253FC
GPIO_PORTF_DIR_R   EQU 0x40025400
GPIO_PORTF_AFSEL_R EQU 0x40025420
GPIO_PORTF_PUR_R   EQU 0x40025510
GPIO_PORTF_DEN_R   EQU 0x4002551C
GPIO_PORTF_LOCK_R  EQU 0x40025520
GPIO_PORTF_CR_R    EQU 0x40025524
GPIO_LOCK_KEY      EQU 0x4C4F434B  ; Unlocks the GPIO_CR register
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       IMPORT  TExaS_Init
       THUMB
       AREA    DATA, ALIGN=2
;global variables go here


      AREA    |.text|, CODE, READONLY, ALIGN=2
      THUMB

      EXPORT  Start

Start
 ; TExaS_Init sets bus clock at 80 MHz
	   BL  TExaS_Init
; voltmeter, scope on PD3
 ; Initialization goes here
	   LDR R0, =SYSCTL_RCGCGPIO_R	;enables clock for Port E and Port F
	   LDRB R1, [R0]
	   ORR R1, #0x30
	   STRB R1, [R0]
	   
	   NOP							;stabilize clock for period of 25 ns
	   NOP

	   LDR R0, =GPIO_PORTE_DIR_R	;set PE2 as output, PE1 as input
	   LDRB R1, [R0]
	   BIC R1, #0x02
	   ORR R1, #0x04
	   STRB R1, [R0]
	   
	   LDR R0, =GPIO_PORTF_DIR_R	;set PF4 as input
	   LDRB R1, [R0]
	   BIC R1, #0x10
	   STRB R1, [R0]	 

	   LDR R0, =GPIO_PORTF_PUR_R	;enable pull-up resistor(s) for PF4
	   LDRB R1, [R0]
	   ORR R1, #0x10					;friendly?
	   STRB R1, [R0]
	   
	   LDR R0, =GPIO_PORTE_AFSEL_R	;enable interrupts for PE2 and PE1 --> Dr.Y said not needed?
	   LDR R1, [R0]
	   BIC R1, #0x06
	   STR R1, [R0]
	   
	   LDR R0, =GPIO_PORTF_AFSEL_R	;enable interrupts for PF4 --> Dr.Y said not needed?
	   LDR R1, [R0]
	   BIC R1, #0x10
	   STR R1, [R0]
	   
	   LDR R0, =GPIO_PORTE_DEN_R	;enable digital function within program for PE2 and PE1
	   LDRB R1, [R0]
	   ORR R1, #0x06
	   STRB R1, [R0]
	   
	   LDR R0, =GPIO_PORTF_DEN_R	;enable digital function within program for PF4
	   LDRB R1, [R0]
	   ORR R1, #0x10
	   STRB R1, [R0]
	   
	   MOV R12, #30					;R12 = 30 --> references beginning duty cycle
	   LDR R11, =100000				;R11 = 100,000 --> multiplier for duty cycle in Delay subroutine
	   MOV R10, #100				;R10 = 100 --> maximum value for % duty cycle
	   MOV R9, #0					;R9 = 0 --> comparator for register values in breathing subroutine
	   LDR R8, =2000				;R8 = 200 --> multiplier for breathing in Delay subroutine
	   
      CPSIE  I    ;TExaS voltmeter, scope runs on interrupts
	   
loop  
; main engine goes here
		BL DutyCycle	;subroutine to adjust and check duty cycle
		BL Breathe		;subroutine to make LED breathe when necessary
		B loop

DutyCycle
		PUSH {LR, R6}
		LDR R0, =GPIO_PORTE_DATA_R	 ;Read data register for Port E
		LDR R1, [R0]
		AND R1, #0x02
		CMP R1, #0		;Check if PE1 is on
		BEQ Back
		BL Check		;Increase duty cycle by 20. If currently 90, reset to 10.
	 
Back					;Subroutine to toggle PE2/LED
		MOV R1, #0x04	
		STR R1, [R0]
		MUL R0, R12, R11
		CMP R0, #0
		BL Delay
		
		LDR R0, =GPIO_PORTE_DATA_R
		MOV R1, #0x00
		STR R1, [R0]
		SUB R9, R10, R12
		MUL R0, R9, R11
		BL Delay	
		
		POP {LR, R6}
		BX LR
	 
Check					;Subroutine to adjust duty cycle
		LDR R1, [R0]
		AND R1, #0x02
		CMP R1, #0
		BNE Check
		
		ADD R12, #20
		CMP R10, R12
		BGT Final
		SUB R12, R10
		
Final					;Return to main engine/subroutine --> "loop"
		BX LR
		
Breathe					;Subroutine for breathing
		PUSH {LR, R7}
		MOV R3, #0
		
Hop
		LDR R0, =GPIO_PORTF_DATA_R
		LDR R1, [R0]
		AND R1, #0x10
		CMP R1, #0
		BNE Leap		;If switch not pressed, exit subroutine
		
		LDR R0, =GPIO_PORTE_DATA_R
		MOV R1, #0x04
		STR R1, [R0]
		CMP R9, #0
		BLE Roll		;Don't turn on if duty cycle set to 0
		
		MUL R0, R9, R8	;Turn on for period of duty cycle on
		BL Delay

Roll
		LDR R0, =GPIO_PORTE_DATA_R
		MOV R1, #0x00
		STR R1, [R0]
		SUB R5, R10, R9
		CMP R5, #0
		BLE Decide
		
		MUL R0, R5, R8	;Turn off for period of duty cycle off
		BL Delay
		
Decide					;Check if we should increase/decrease brightness
		CMP R3, #0
		BNE HighZero

SubZero					;Increase duty cycle % by 1 till max--100--reached
		ADD R9, #1
		CMP R10, R9
		BGE Reset
		SUB R9, #1
		ADD R3, #1
		B Reset
		
HighZero				;Decrease duty cycle % by 1 till min--0--reached
		SUB R9, #1
		CMP R9, #0
		BGE Reset
		ADD R9, #1
		ADD R3, #-1
		
Reset					;Subroutine to check if PF4 still on --> Negative Logic
		LDR R0, =GPIO_PORTF_DATA_R
		LDR R1, [R0]
		AND R1, #0x10
		CMP R1, #0
		BEQ Hop
		
Leap	POP {LR, R7}	;Return to main engine --> "loop"
		BX LR
	 
Delay						;Subroutine for Delay
		SUBS R0, R0, #1
		BNE Delay
		BX LR
    

     
     ALIGN      ; make sure the end of this section is aligned
     END        ; end of file
