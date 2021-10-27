#include "scheduler.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <time.h> 

#define QUEUE_SIZE 1000

//  Job array, mutex locks, condition variables?
    // Create job struct
    extern struct Jobs {
        char job_name[25];
        int cpu_time;
        int priority;
        int hour;     //submission time
        int min;
        int sec;
        double sub_time_sec;//submission time in current seconds
        double finish_time; //finish time in current seconds
        double sub_time;    //time when job is submitted in cpu time
        double exec_time;   //time when job begins to be serviced
        char progress[25];
    };  
    // mutex lock for job queue
    extern clock_t t;
    extern pid_t pid;
    extern pthread_mutex_t lock;
    extern pthread_cond_t new_job; //marks if there is a new job
    extern pthread_cond_t nfull;   //marks if queue is not full
    extern pthread_cond_t nempty;  //marks if queue is not empty
    extern pthread_cond_t full;    //marks if queue is full
    extern pthread_cond_t empty;   //marks if queue is empty
    extern struct Jobs queue[QUEUE_SIZE];  //shared job queue between scheduler and dispatcher
    extern char g_job_name[25];    //global name for new jobs
    extern int g_priority = 10;    //global priority for new jobs
    extern int g_cpu_time = 10;    //global cpu time for new jobs
    extern int jobs_queued = 0;
    extern int expected_wait = 0;
    extern int sort_type = 0;      // 0 = fcfs, 1 = sjf, 2 = priority
    extern int quit = 0;           //signals threads to quit
    extern int total_jobs_sub = 0;     //total jobs submitted
    extern int total_cpu_time = 0;
    extern struct timeval program_start_time;
    extern double tot_turnaround = 0;  //total turnaround time
    extern double tot_wait_time = 0;   //total wait time - since the policies are not preemptive, this is the same as total response time

// swaps the pointers of two objects
void swap(struct Jobs *x, struct Jobs *y) { 
    struct Jobs temp = *x;
    *x = *y;
    *y = temp;
}

// schedules jobs with shortest job first
//  slide the new job into the last position, then sort based on expected execution time
//  use selection sort for this task, based off of https://www.geeksforgeeks.org/selection-sort/
//  TODO change this up a bit
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
//  TODO change this up a bit
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
//  TODO change this up a bit
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
void *schedule_jobs(void *param) {
    while (quit == 0) {
        pthread_mutex_lock(&lock);
        pthread_cond_wait(&new_job, &lock);
        //create new job
        struct Jobs temp;
        strcpy(temp.job_name, g_job_name);
        temp.cpu_time = g_cpu_time;
        temp.priority = g_priority;
        total_cpu_time = total_cpu_time + g_cpu_time;
        //find arrival time of job
        //inflate cpu arrival time in case jobs arrive on same cpu cycle
        //this ensures that even if 5 jobs arrive on the same cpu cycle, they 
        //will be recorded in order of arrival
        temp.sub_time = (double) t + (double) jobs_queued;
        time_t now;
        time(&now);
        struct timeval t1;
        gettimeofday(&t1, NULL);
        temp.sub_time_sec = t1.tv_sec;
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