// Lab6.c
// Runs on TM4C123
// Use SysTick interrupts to implement a 4-key digital piano
// MOOC lab 13 or EE319K lab6 starter
// Program written by: Keshav Narasimhan & Sanjay Gorur
// Date Created: 3/6/17 
// Last Modified: 1/17/21  
// Lab number: 6
// Hardware connections
// TO STUDENTS "REMOVE THIS LINE AND SPECIFY YOUR HARDWARE********


#include <stdint.h>
#include "../inc/tm4c123gh6pm.h"
#include "../inc/LaunchPad.h"
#include "../inc/CortexM.h"
#include "Sound.h"
#include "Key.h"
#include "Music.h"
#include "TExaS.h"

void DAC_Init(void);         // your lab 6 solution
void DAC_Out(uint8_t data);  // your lab 6 solution
uint8_t Testdata;

// lab video Lab6_voltmeter
int voltmetermain(void){ //voltmetermain(void){     
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  DAC_Init(); // your lab 6 solution
  Testdata = 63;
  EnableInterrupts();
  while(1){                
    Testdata = (Testdata+1)&0x3F;
    DAC_Out(Testdata);  // your lab 6 solution
  }
}

// lab video Lab6_static
int staticmain(void){   uint32_t last,now;  
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  LaunchPad_Init();
  DAC_Init(); // your lab 6 solution
  Testdata = 63;
  EnableInterrupts();
  last = LaunchPad_Input();
  while(1){                
    now = LaunchPad_Input();
    if((last != now)&&now){
       Testdata = (Testdata+1)&0x3F;
       DAC_Out(Testdata); // your lab 6 solution
    }
    last = now;
    Clock_Delay1ms(25);   // debounces switch
  }
}



//**************Lab 6 solution below*******************

uint32_t h_beat = 0;
void HeartBeat()
{
	h_beat++;
	if (h_beat & 0x10000)
	{
		GPIO_PORTF_DATA_R ^= 0x04;
	}
}

int main(void){      
  TExaS_Init(SW_PIN_PE3210,DAC_PIN_PB3210,ScopeOn);    // bus clock at 80 MHz
  Key_Init();
  LaunchPad_Init();
  Sound_Init();
  // other initialization
	SYSCTL_RCGCGPIO_R |= 0x20;
	__nop();
	__nop();
	
	GPIO_PORTF_DIR_R |= 0x04;
	GPIO_PORTF_DEN_R |= 0x04;
	
	EnableInterrupts();
	
  while(1){ 
		
		HeartBeat();
		
		uint8_t btn_input = Key_In();
		
		if (btn_input == 0)
		{
			Sound_Start(0);								// No Sound
		}
		else if (btn_input == 1)
		{
			Sound_Start(1790);						// F4 = 698.5 Hz
		}
		else if (btn_input > 1 && btn_input < 4)
		{
			Sound_Start(1420);						// A4 = 880 Hz
		}
		else if (btn_input >= 4 && btn_input < 8)
		{
			Sound_Start(1194);						// C5 = 1046.5 Hz
		}
		else
		{
			Sound_Start(895);							// F5 = 1396.5 Hz
		}
  }           
}


