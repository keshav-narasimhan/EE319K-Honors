// ****************** Lab2.c ***************
// Program written by: put your names here
// Date Created: 1/18/2017 
// Last Modified: 1/17/2021 
// Put your name and EID in the next two line
char Name[] = "Keshav Narasimhan";
char EID[] = "______";
// Brief description of the Lab: 
// An embedded system is capturing temperature data from a
// sensor and performing analysis on the captured data.
// The controller part of the system is periodically capturing size
// readings of the temperature sensor. Your task is to write three
// analysis routines to help the controller perform its function
//   The three analysis subroutines are:
//    1. Calculate the mean of the temperature readings 
//       rounded down to the nearest integer
//    2. Convert from Centigrate to Farenheit using integer math 
//    3. Check if the captured readings are a non-increasing monotonic series
//       This simply means that the readings are sorted in non-increasing order.
//       We do not say "decreasing" because it is possible for consecutive values
//       to be the same, hence the term "non-increasing". The controller performs 
//       some remedial operation and the desired effect of the operation is to 
//       raise the the temperature of the sensed system. This routine helps 
//       verify whether this has indeed happened
#include <stdint.h>
#define True 1
#define False 0

// Return the computed Mean
// Inputs: Data is an array of 16-bit unsigned distance measurements
//         N is the number of elements in the array
// Output: Average of the data
// Notes: you do not need to implement rounding
uint16_t Average(const uint16_t Data[],const uint32_t N){
		
	uint16_t dist_sum = 0;						// 16 bit, unsigned int var that will hold the sum of all distance measurements in Data array
	for (int a = 0; a < N; a++)				// loop through all N elements in array
	{
		dist_sum += Data[a];					// add all distance measurements in array and hold sum in dist_sum (mm)
	}
	uint16_t dist_avg = dist_sum/N;		// define new 16 bit, unsigned int var that will compute the average distance
   	return dist_avg;									// return the average distance by returning dist_avg (mm)

}

// Convert temperature in Farenheit to temperature in Centigrade
// Inputs: temperature in Farenheit 
// Output: temperature in Centigrade
// Notes: you do not need to implement rounding
int16_t FtoC(int16_t const TinF){
	
	int16_t deg_cel = TinF - 32;		// define new 16 bit int, deg_cel, that will compute the Celsius Temp.
	deg_cel = deg_cel * 5/9;				// use formula and compute the Celsius Temp. using the input Fahrenheit Temp.
	return deg_cel;									// return deg_cel (Celsius Temp.) in degrees Celsius

}

// Return True of False based on whether the readings
// are an increasing monotonic series
// Inputs: Readings is an array of 16-bit distance measurements
//         N is the number of elements in the array
// Output: true if monotonic increasing, false if nonmonotonic
int IsMonotonic(uint16_t const Data[], int32_t const N){

	for (int a = 0; a < N - 1; a++)				// loop through N - 1 elements in Data array to compare each data pt. to see if increasing
	{
		if (Data[a + 1] < Data[a])				// if next distance is less than current distance measurement, we know it isn't strictly increasing
		{
			return False;									// since not strictly increasing, return False
		}
	}
	return True;													// reached end of array without finding any decreasing distance measurements, therefore return True

}


