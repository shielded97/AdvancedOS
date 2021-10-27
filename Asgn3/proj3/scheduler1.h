/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: global.h
    Function: header file for scheduler.c, defining swap(), 
        sjf_scheduler(), fcfs_scheduler(), pri_scheduler()
    March 13, 2021
*/

#ifndef SCHEDULER1_H_INCLUDED
#define SCHEDULER1_H_INCLUDED

// swaps the pointers of two objects
void swap(struct Jobs *x, struct Jobs *y);

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void sjf_scheduler();

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void fcfs_scheduler();

// schedules jobs with priority
//  Lower number is higher priority
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void pri_scheduler();

// Function: schedule_jobs()
void *schedule_jobs(void *param);

#endif