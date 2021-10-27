/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: scheduler1.c
    Function: provides scheduling functionality to aubatchSep.c 
    Relies on scheduler1.h, global.h, and aubatchSep.c
    March 13, 2021
*/

#include "global.h"
#include "scheduler1.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <time.h> 

#define QUEUE_SIZE 1000

// swaps the pointers of two objects
// based off of https://www.geeksforgeeks.org/selection-sort/
void swap(struct Jobs *x, struct Jobs *y) { 
    struct Jobs temp = *x;
    *x = *y;
    *y = temp;
}

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void sjf_scheduler() {
    int i;
    int j;
    int min_index;
    // sort from the second element because we don't want to change the running job
    for (i = 1; i < jobs_queued-1; i++) {
        min_index = i;
        for (j = i+1; j < jobs_queued; j++) {
            if (queue[j].cpu_time < queue[min_index].cpu_time) {
                min_index = j;
            }
        }
        swap(&queue[min_index], &queue[i]);
    }
}

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void fcfs_scheduler() {
    int i;
    int j;
    int first_index;
    // sort from the second element because we don't want to change the running job
    for (i = 1; i < jobs_queued-1; i++) {
        first_index = i;
        for (j = i+1; j < jobs_queued; j++) {
            if ((double) queue[j].sub_time < (double) queue[first_index].sub_time) {
                first_index = j;
            }
        }
        swap(&queue[first_index], &queue[i]);
    }
}
// schedules jobs with priority
//  Lower number is higher priority
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
void pri_scheduler() {
    int i;
    int j;
    int min_index;
    // sort from the second element because we don't want to change the running job
    for (i = 1; i < jobs_queued-1; i++) {
        min_index = i;
        for (j = i+1; j < jobs_queued; j++) {
            if (queue[j].priority < queue[min_index].priority) {
                min_index = j;
            }
        }
        swap(&queue[min_index], &queue[i]);
    }
}

// Function: schedule_jobs()
//  in charge of scheduling jobs into the job queue
//  called from main() in aubatchSep.c
void *schedule_jobs(void *param) {
    while (quit == 0) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&new_job, &lock);
        //create new job
        struct Jobs temp;
        strcpy(temp.job_name, g_job_name);
        temp.cpu_time = g_cpu_time;
        temp.priority = g_priority;
        
        //find arrival time of job
        //inflate cpu arrival time in case jobs arrive on same cpu cycle
        //this ensures that even if 5 jobs arrive on the same cpu cycle, they 
        //will be recorded in order of arrival
        
        time_t now;
        time(&now);
        struct timeval t1;
        gettimeofday(&t1, NULL);
        double sub_time = (double) t1.tv_sec + ((double) t1.tv_usec * 0.000001);
        temp.sub_time = sub_time;
        struct tm *local = localtime(&now);
        int hour = local->tm_hour;
        int minutes = local->tm_min;
        int seconds = local->tm_sec;
        temp.min = minutes;
        temp.sec = seconds;
        temp.hour = hour;
        strcpy(temp.progress, "waiting");

        //  wait until queue is not full
        while (jobs_queued == QUEUE_SIZE) {
            pthread_cond_wait(&nfull, &lock);
        }

        //  Add new job to end of queue
        //  This suffices for a FCFS implementation, but
        //  also must be done for the rest of the policies
        queue[jobs_queued] = temp;
        jobs_queued++;
        total_jobs_sub++;

        if (sort_type == 1) {  //sjf
            sjf_scheduler();
        }
        else if (sort_type == 2) {  //priority
            pri_scheduler();
        }
        else {
            sort_type = 0;
        }
        //  if thread is full, signal full
        if (jobs_queued == QUEUE_SIZE) {
            pthread_cond_signal(&full);
        }
        //  signal that queue is not empty
        pthread_cond_signal(&nempty);
        //  unlock mutex for future
        pthread_mutex_unlock(&lock);
    }
}