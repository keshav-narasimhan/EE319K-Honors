;****************** main.s ***************
; Program initially written by: Yerraballi and Valvano
; Author: Keshav Narasimhan
; Date Created: 1/15/2018 
; Last Modified: 1/17/2021 
; Brief description of the program: Solution to Lab1
; The objective of this system is to implement odd-bit counting system
; Hardware connections: 
;  Output is positive logic, 1 turns on the LED, 0 turns off the LED
;  Inputs are negative logic, meaning switch not pressed is 1, pressed is 0
;    PE0 is an input 
;    PE1 is an input 
;    PE2 is an input 
;    PE4 is the output
; Overall goal: 
;   Make the output 1 if there is an even number of switches pressed, 
;     otherwise make the output 0

; The specific operation of this system 
;   Initialize Port E to make PE0,PE1,PE2 inputs and PE4 an output
;   Over and over, read the inputs, calculate the result and set the output
; PE2  PE1 PE0  | PE4
; 0    0    0   |  0    3 switches pressed, odd 
; 0    0    1   |  1    2 switches pressed, even
; 0    1    0   |  1    2 switches pressed, even
; 0    1    1   |  0    1 switch pressed, odd
; 1    0    0   |  1    2 switches pressed, even
; 1    0    1   |  0    1 switch pressed, odd
; 1    1    0   |  0    1 switch pressed, odd
; 1    1    1   |  1    no switches pressed, even
;There are 8 valid output values for Port E: 0x00,0x11,0x12,0x03,0x14,0x05,0x06, and 0x17. 

; NOTE: Do not use any conditional branches in your solution. 
;       We want you to think of the solution in terms of logical and shift operations

GPIO_PORTE_DATA_R  EQU 0x400243FC
GPIO_PORTE_DIR_R   EQU 0x40024400
GPIO_PORTE_DEN_R   EQU 0x4002451C
SYSCTL_RCGCGPIO_R  EQU 0x400FE608

       THUMB
       AREA    DATA, ALIGN=2
;global variables go here
      ALIGN
      AREA    |.text|, CODE, READONLY, ALIGN=2
      EXPORT  Start
Start
     ;code to run once that initializes PE4,PE2,PE1,PE0
	 
	 ; activates Clock E
	 LDR 	R0,=SYSCTL_RCGCGPIO_R
	 LDR	R1,[R0]
	 ORR	R1,#0x10
	 STR 	R1,[R0]
	 
	 ; wait for 2 clock cycles to stabilize clock
	 NOP
	 NOP		
	 
	 ; specify which pins are inputs/outputs
	 ; PE0, PE1, PE2 = inputs
	 ; PE4 = output
	 LDR 	R0,=GPIO_PORTE_DIR_R
	 LDR	R1,[R0]
	 BIC 	R1, #0x07	; makes PE0, PE1, and PE2 inputs (friendly)
	 ORR	R1, #0x10	; makes PE4 output (friendly)
	 STR	R1,[R0]
	 
	 ; specify which pins are enabled as data pins
	 ; PE0, PE1, PE2, PE4 = data pins
	 LDR	R0,=GPIO_PORTE_DEN_R
	 LDR	R1,[R0]
	 ORR	R1,#0x17	; bits 0, 1, 2, 4 are enabled (friendly)
	 STR	R1,[R0]
	 
	 ; load R0 with address of PORT E DATA Register
	 LDR 	R0,=GPIO_PORTE_DATA_R
loop
      ;code that runs over and over
	  
	  ; PF0
	  LDR	R1,[R0]		; load data into R1
	  AND	R1,#0x1		; PE0 info is in bit 0, so we'll check if it is a 0/1 with a mask
	  
	  ; PF1
	  LDR	R2,[R0]		; load data into R2
	  LSR	R2,#0x1		; PE1 info is in bit 1, so we'll shift right once to make it the LSB
	  AND	R2,#0x1		; we'll check if bit 1 is a 0/1 with a mask
	  
	  ; PF2
	  LDR	R3,[R0]		; load data into R3
	  LSR	R3,#0x2		; PE2 info is in bit 2, so we'll shift right twice to make it the LSB
	  AND	R3,#0x1		; we'll check if bit 1 is a 0/1 with a mask
	  
	  ; R1 = PE0, R2 = PE1, R3 = PE2
	  ; we can use XOR logic in order to determine if output (PE4) should be high/low
	  EOR 	R1, R2
	  EOR	R1, R3
	  
	  ; whether the output should be 0/1 is stored in R4 due to the EOR operations
	  ; shift this value in order to match it up with bit 4
	  LSL	R1, #0x4
	  LDR	R2, [R0]
	  AND	R2, #0x0F	; preserves bits 0,1,2 original values
	  ADD	R2, R1		; changes bit 4 only by setting it as whatever stored in R1
	  STR	R2, [R0]	; store it back to see resulting output
    B    loop

    
    ALIGN        ; make sure the end of this section is aligned
    END          ; end of file
          