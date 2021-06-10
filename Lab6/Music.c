// Music.c
// This program can use timer0A and timer1A ISR  
// playing your favorite song.
//
// For use with the TM4C123
// EE319K lab6 extra credit
// Program written by: Keshav Narasimhan & Sanjay Gorur
// 1/17/21



#include "Sound.h"
#include "Music.h"
#include "DAC.h"
#include <stdint.h>
#include <stdlib.h>
#include "../inc/tm4c123gh6pm.h"

void Timer0A_Init(void(*task)(void), uint32_t period);

extern int indx = 0;
note_t Song[42] = {
	{C0, qg},
	//{0, g},
	{C0, q},
	
	{G, qg},
	//{0, g},
	{G, q},
	
	{A, qg},
	//{0, g},
	{A, q},
	
	{G, h},
	
	{F, qg},
	//{0, g},
	{F, q},
	
	{E, qg},
	//{0, g},
	{E, q},
	
	{D, qg},
	//{0, g},
	{D, q},
	
	{C0, h},
	
	{G, qg},
	//{0, g},
	{G, q},
	
	{F, qg},
	//{0, g},
	{F, q},
	
	{E, qg},
	//{0, g},
	{E, q},
	
	{D, h},
	
	{G, qg},
	//{0, g},
	{G, q},
	
	{F, qg},
	//{0, g},
	{F, q},
	
	{E, qg},
	//{0, g},
	{E, q},
	
	{D, h},
	
	{C0, qg},
	//{0, g},
	{C0, q},
	
	{G, qg},
	//{0, g},
	{G, q},
	
	{A, qg},
	//{0, g},
	{A, q},
	
	{G, h},
	
	{F, qg},
	//{0, g},
	{F, q},
	
	{E, qg},
	//{0, g},
	{E, q},
	
	{D, qg},
	//{0, g},
	{D, q},
	
	{C0, h}
	
	//{0, 0}
	
};
/*
// Timer0A actually controls outputting to DAC
void Timer0A_Handler(void){
    
	TIMER0_ICR_R = TIMER_ICR_TATOCINT;
	if (Song[indx].duration == 0)
	{
		TIMER0_IMR_R = 0x00;
		//TIMER0_CTL_R = 0x00;
		//Music_StopSong();
		return;
	}
	Sound_Start(Song[indx].pitch);
	TIMER0_TAILR_R = Song[indx].duration;
	indx += 1;
	
}

  // Timer1A acts as a metronome for the song(s)
void Timer1A_Handler(void){
    // write this
  // extra credit


}
*/

#define NVIC_EN0_INT19          0x00080000  // Interrupt 19 enable
#define NVIC_EN0_INT21          0x00200000  // Interrupt 21 enable

// ***************** Timer_Init ****************
// Activate Timer0A and Timer1A interrupts to run user task periodically
// Inputs: period0 in nsec
//         period1 in msec
// Outputs: none
void Timer_Init(unsigned int period0){ uint32_t volatile delay;
   
	SYSCTL_RCGCTIMER_R |= 0x01;
	delay = 0;
	TIMER0_CTL_R = 0x00;
	TIMER0_CFG_R = 0x00;
	TIMER0_TAMR_R = 0x02;
	TIMER0_TAILR_R = period0 - 1;
	TIMER0_TAPR_R = 0x00;
	TIMER0_ICR_R = 0x01;
	TIMER0_IMR_R = 0x01;
	NVIC_PRI4_R = (NVIC_PRI4_R & 0x00FFFFFF) | 0x80000000;
	NVIC_EN0_R |= NVIC_EN0_INT19;
	TIMER0_CTL_R = 0x01;

	/*
	SYSCTL_RCGCTIMER_R |= 0x02;   // 0) activate TIMER1
  TIMER1_CTL_R = 0x00;    // 1) disable TIMER1A during setup
  TIMER1_CFG_R = 0x00;    // 2) configure for 32-bit mode
  TIMER1_TAMR_R = 0x02;   // 3) configure for periodic mode, default down-count settings
  TIMER1_TAILR_R = period1 - 1;    // 4) reload value
  TIMER1_TAPR_R = 0x00;            // 5) bus clock resolution
  TIMER1_ICR_R = 0x01;    // 6) clear TIMER1A timeout flag
  TIMER1_IMR_R = 0x01;    // 7) arm timeout interrupt
  NVIC_PRI5_R = (NVIC_PRI5_R & 0xFFFF00FF) | 0x00008000; // 8) priority 4
  NVIC_EN0_R = 1 << 21;           // 9) enable IRQ 21 in NVIC
  TIMER1_CTL_R = 0x01;    // 10) enable TIMER1A
*/

}
void Timer_Stop(void){ 
    
	TIMER0_CTL_R = 0x00;
	TIMER0_TAILR_R = 0x00;

}

void Music_Init(void){
	
	//TIMER1_IMR_R = 0x00;
	Timer_Init(bus/4);
	TIMER0_IMR_R = 0x00;
	indx = 0;
	
}

void changeNote (void)
{
	
	indx = (indx + 1) % 42;
	Sound_Start(Song[indx].pitch);
	Timer0A_Init(changeNote, Song[indx].duration);
	
}

// Play song, while button pushed or until end
void Music_PlaySong(){
  
	if (indx != 0)
	{
		return;
	}
	//indx = 0;
	Sound_Start(Song[indx].pitch);
	Timer0A_Init(changeNote, Song[indx].duration);
	//indx = (indx + 1) % 42;
}

// Stop song
void Music_StopSong(void){
  
	indx = 0;
	TIMER0_CTL_R = 0x00;
	
} 
