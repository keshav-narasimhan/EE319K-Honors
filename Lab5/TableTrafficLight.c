// TableTrafficLight.c solution to EE319K Lab 5, spring 2021
// Runs on TM4C123
// Moore finite state machine to operate a traffic light.  
// Sanjay Gorur & Keshav Narasimhan
// March 11, 2021

/* 

 Copyright 2021 by Jonathan W. Valvano, valvano@mail.utexas.edu
    You may use, edit, run or distribute this file
    as long as the above copyright notice remains
 THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 VALVANO SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL,
 OR CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 For more information about my classes, my research, and my books, see
 http://users.ece.utexas.edu/~valvano/
 */

// east/west red light connected to PB5
// east/west yellow light connected to PB4
// east/west green light connected to PB3
// north/south facing red light connected to PB2
// north/south facing yellow light connected to PB1
// north/south facing green light connected to PB0

// pedestrian detector connected to PE2 (1=pedestrian present)
// north/south car detector connected to PE1 (1=car present)
// east/west car detector connected to PE0 (1=car present)

// "walk" light connected to PF3-1 (built-in white LED)
// "don't walk" light connected to PF1 (built-in red LED)

/*
Inputs:
	- 0x0 (000) = no sensor
	- 0x1 (001) = cars from west
	- 0x2 (010) = cars from south
	- 0x3 (011) = cars from west & south
	- 0x4 (100) = pedestrians
	- 0x5 (101) = pedestrians & cars from west
	- 0x6 (110) = pedestrians & cars from south
	- 0x7 (111) = pedestrians & cars from west & cars from south
*/

/*
Port B Outputs:
	- 0x24 (100 100) = east: red, south: red
	- 0x14 (010 100) = east: yellow, south: red
	- 0x0C (001 100) = east: green, south: red
	- 0x21 (100 001) = east: red, south: green
	- 0x22 (100 010) = east: red, south: yellow
*/

/*
Port F Outputs:
	- 0x02 (0010) = red walk light
	- 0x00 (0000) = walk light off (for flashing purposes)
	- 0x0E (1110) = white walk light
*/


#include <stdint.h>
#include "SysTick.h"
#include "TExaS.h"
#include "../inc/tm4c123gh6pm.h"


/*
State Declarations Using Pointers
*/
#define all_red  			&FSM[0]
#define go_west				&FSM[1]
#define wait_west_1 	&FSM[2]
#define wait_west_2		&FSM[3]
#define turn_to_west	&FSM[4]
#define go_south			&FSM[5]
#define wait_south_1	&FSM[6]
#define wait_south_2	&FSM[7]
#define turn_to_south	&FSM[8]
#define walk		 			&FSM[9]	
#define walk_flash_1	&FSM[10]
#define walk_flash_2	&FSM[11]
#define walk_flash_3	&FSM[12]
#define walk_flash_4	&FSM[13]
#define walk_flash_5	&FSM[14]
#define walk_flash_6	&FSM[15]
#define end_walk			&FSM[16]
#define turn_to_walk	&FSM[17]

/*
_state Struct Declaration
*/
typedef struct state
{
	uint32_t portB_Output;
	uint32_t portF_Output;
	uint32_t delayTime;
	struct state *next_state[8];
} _state;

/*
Declare an Array of _state Structs called FSM
	- will hold each of 18 states
*/
_state FSM[18] = 
{
	
	// all_red 
	{0x24, 0x02, 100, {all_red, go_west, go_south, go_south, walk, walk, walk, walk}},
	
	// go_west
	{0x0C, 0x02, 300, {wait_west_1, go_west, wait_west_1, wait_west_1, wait_west_2, wait_west_2, wait_west_2, wait_west_2}},
		
	// wait_west_1
	{0x14, 0x02, 50, {all_red, all_red, turn_to_south, turn_to_south, turn_to_south, turn_to_south, turn_to_south, turn_to_south}},
		
	// wait_west_2
	{0x14, 0x02, 50, {turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk}},
		
	// turn_to_west
	{0x24, 0x02, 100, {go_west, go_west, go_west, go_west, go_west, go_west, go_west, go_west}},
	
	// go_south
	{0x21, 0x02, 300, {wait_south_1, wait_south_1, go_south, wait_south_1, wait_south_2, wait_south_2, wait_south_2, wait_south_2}},
		
	// wait_south_1
	{0x22, 0x02, 50, {all_red, turn_to_west, all_red, turn_to_west, turn_to_west, turn_to_west, turn_to_west, turn_to_west}},
		
	// wait_south_2
	{0x22, 0x02, 50, {turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_walk, turn_to_west}},
		
	// turn_to_south
	{0x24, 0x02, 100, {go_south, go_south, go_south, go_south, go_south, go_south, go_south, go_south}},
		
	// walk
	{0x24, 0x0E, 250, {walk_flash_1, walk_flash_1, walk_flash_1, walk_flash_1, walk, walk_flash_1, walk_flash_1, walk_flash_1}},
		
	// walk_flash_1
	{0x24, 0x02, 40, {walk_flash_2, walk_flash_2, walk_flash_2, walk_flash_2, walk_flash_2, walk_flash_2, walk_flash_2, walk_flash_2}},
		
	// walk_flash_2
	{0x24, 0x00, 40, {walk_flash_3, walk_flash_3, walk_flash_3, walk_flash_3, walk_flash_3, walk_flash_3, walk_flash_3, walk_flash_3}},
		
	// walk_flash_3
	{0x24, 0x02, 40, {walk_flash_4, walk_flash_4, walk_flash_4, walk_flash_4, walk_flash_4, walk_flash_4, walk_flash_4, walk_flash_4}},
		
	// walk_flash_4
	{0x24, 0x00, 40, {walk_flash_5, walk_flash_5, walk_flash_5, walk_flash_5, walk_flash_5, walk_flash_5, walk_flash_5, walk_flash_5}},
		
	// walk_flash_5
	{0x24, 0x02, 40, {walk_flash_6, walk_flash_6, walk_flash_6, walk_flash_6, walk_flash_6, walk_flash_6, walk_flash_6, walk_flash_6}},
		
	// walk_flash_6
	{0x24, 0x00, 40, {end_walk, end_walk, end_walk, end_walk, end_walk, end_walk, end_walk, end_walk}},
		
	// end_walk
	{0x24, 0x02, 40, {all_red, turn_to_west, turn_to_south, turn_to_south, turn_to_walk, turn_to_west, turn_to_south, turn_to_south}},
	
	// turn_to_walk
	{0x24, 0x02, 100, {walk, walk, walk, walk, walk, walk, walk, walk}}
	
};

void DisableInterrupts(void);
void EnableInterrupts(void);

/*
Pin Declarations
	- PB543210: GPIO_PORTB_DATA_R bits 5 - 0
	- PE210: GPIO_PORTE_DATA_R bits 2 - 0
	- PF321: GPIO_PORTF_DATA_R bits 3 - 1 
*/
#define PB543210                (*((volatile uint32_t *)0x400050FC)) // bits 5-0
#define PE210                   (*((volatile uint32_t *)0x4002401C)) // bits 2-0
#define PF321                   (*((volatile uint32_t *)0x40025038)) // bits 3-1


void LogicAnalyzerTask(void){
  UART0_DR_R = 0x80|GPIO_PORTB_DATA_R;
}
int main(void){ 
 /*	
	// for simulation
	volatile uint32_t delay;
  DisableInterrupts();
  //TExaS_Init(&LogicAnalyzerTask);
  PLL_Init(); // PLL on at 80 MHz
  SYSCTL_RCGC2_R |= 0x32; // Ports B,E,F
  delay = SYSCTL_RCGC2_R;
 */
	
// /*	
	// for board
	volatile uint32_t delay;
  DisableInterrupts();
  //TExaS_Init(&LogicAnalyzerTask);
  PLL_Init(); // PLL on at 80 MHz
  SYSCTL_RCGC2_R |= 0x32; // Ports B,E,F
  delay = SYSCTL_RCGC2_R;
// */
 
	
  EnableInterrupts();						// enable interrupts
	
	volatile int NOP;							// stabilize clock with a dummy variable
	NOP++;
	NOP++;
	/*
	Initialization of Important Registers
		- DIR for Ports B, E, F
		- DEN for Ports B, E, F
	*/
	GPIO_PORTB_DIR_R |= 0x3F;
	GPIO_PORTB_DEN_R |= 0x3F;
	
	GPIO_PORTE_DIR_R &= ~0x07;
	GPIO_PORTE_DEN_R |= 0x07;
	
	GPIO_PORTF_DIR_R |= 0x0E;
	GPIO_PORTF_DEN_R |= 0x0E;
	
	/*
	Declare Variables to Use in While Loop
		- input_val = will hold PE210 val (pedestrians, south, west)
		- *state_pt = state pointer var that will start as all_red & change based on input_val
	*/
	uint32_t input_val;
	_state *state_pt; 
	state_pt = all_red;
    
  while(1)
	{
			// output
			PB543210 = state_pt->portB_Output;
			PF321 = state_pt->portF_Output;
		
			// delay
			SysTick_Wait10ms(state_pt->delayTime);
		
			// input
			input_val = PE210;
		
			// transition to next state
			state_pt = state_pt->next_state[input_val];
  }
}



