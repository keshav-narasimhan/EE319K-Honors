// Sound.c
// This module contains the SysTick ISR that plays sound
// Runs on TM4C123
// Program written by: Keshav Narasimhan & Sanjay Gorur
// Date Created: 3/6/17 
// Last Modified: 1/17/21 
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********

// Code files contain the actual implemenation for public functions
// this file also contains an private functions and private data
#include <stdint.h>
#include "DAC.h"
#include "../inc/tm4c123gh6pm.h"


// Global Vars
uint8_t ind = 0;
uint8_t sin_wave[64] = {32, 35, 38, 41, 44, 47, 49, 52, 54, 56, 58, 
												59, 61, 62, 62, 63, 63, 63, 62, 62, 61, 59, 
												58, 56, 54, 52, 49, 47, 44, 41, 38, 35, 32, 
												29, 26, 23, 20, 17, 15, 12, 10, 8, 6, 5, 3, 
												2, 2, 1, 1, 1, 2, 2, 3, 5, 6, 8, 10, 12, 15,
												17, 20, 23, 26, 29};


// **************Sound_Init*********************
// Initialize digital outputs and SysTick timer
// Called once, with sound/interrupts initially off
// Input: none
// Output: none
void Sound_Init(void){
  
	DAC_Init();
	NVIC_ST_CTRL_R = 0x00;
	NVIC_ST_CURRENT_R = 0x00;
	NVIC_SYS_PRI3_R = (NVIC_SYS_PRI3_R & 0x00FFFFFF) | 0x20000000; // Set priority level to 1
  
}

// **************Sound_Start*********************
// Start sound output, and set Systick interrupt period 
// Sound continues until Sound_Start called again, or Sound_Off is called
// This function returns right away and sound is produced using a periodic interrupt
// Input: interrupt period
//           Units of period to be determined by YOU
//           Maximum period to be determined by YOU
//           Minimum period to be determined by YOU
//         if period equals zero, disable sound output
// Output: none
void Sound_Start(uint32_t period){
  
	if (period == 0)
	{
		NVIC_ST_RELOAD_R = 0;
		return;
	}
	NVIC_ST_RELOAD_R = period - 1;
	NVIC_ST_CTRL_R |= 0x07;

}

// **************Sound_Voice*********************
// Change voice
// EE319K optional
// Input: voice specifies which waveform to play
//           Pointer to wave table
// Output: none
void Sound_Voice(const uint8_t *voice){
  // optional
}
// **************Sound_Off*********************
// stop outputing to DAC
// Output: none
void Sound_Off(void){
  
	NVIC_ST_CTRL_R = 0x00;

}
// **************Sound_GetVoice*********************
// Read the current voice
// EE319K optional
// Input: 
// Output: voice specifies which waveform to play
//           Pointer to current wavetable
const uint8_t *Sound_GetVoice(void){
 // write this
 // optional
  return 0;
}

// Interrupt service routine
// Executed every 12.5ns*(period)
void SysTick_Handler(void){
    
	ind = (ind + 1) % 64;
	DAC_Out(sin_wave[ind]);

}


