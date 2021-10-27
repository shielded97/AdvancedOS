
#ifdef SCHEDULER_H_INCLUDED
#define SCHEDULER_H_INCLUDED

// swaps the pointers of two objects
void swap(struct Jobs *x, struct Jobs *y);

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
//  TODO change this up a bit
void sjf_scheduler();

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
//  TODO change this up a bit
void fcfs_scheduler();

// schedules jobs with priority
//  Lower number is higher priority
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
//  TODO change this up a bit
void pri_scheduler();

// Function: schedule_jobs()
void *schedule_jobs(void *param);

#endif