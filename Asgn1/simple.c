/* read an array of 10 numbers, calculate the average of their square roots, print the result
 * Aidan Lambrecht
 * Advanced Operating Systems
 * Assignment 1: repurposed for Asgn 1 from Adv. OS, written by me 
 */

#include <stdio.h>
#include <math.h>

int main()
{
	int arr[] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	double sum = 0;
	int x;

	for ( x = 0; x < 10; x++) 
	{
		sum = sum + sqrt(arr[x]);
	}

	double avg = sum / 10;

	printf("Average of the square roots of 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 = ");
	printf("%lf \n", avg);
	
	return 0;
}
