/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: global.h
    Function: header file for whole project, defining all global structures and variables 
    March 13, 2021
*/

#ifndef GLOBAL_H_INCLUDED
#define GLOBAL_H_INCLUDED

#ifndef EXTERN
#define EXTERN extern
#endif

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <math.h>
#include <time.h> 

#define QUEUE_SIZE 1000

    struct Jobs {
        char job_name[25];
        double cpu_time;   //estimated cpu_time
        int priority;
        int hour;     //submission time
        int min;
        int sec;
        double sub_time_sec;//submission time in current seconds
        double finish_time; //finish time in current seconds
        double sub_time;    //time when job is submitted in cpu time
        double exec_time;   //time when job begins to be serviced in cpu time
        //double fin_time;    //time when job completes in cpu time
        char progress[25];
    };  

    // mutex lock for job queue
    EXTERN clock_t t;
    EXTERN clock_t prog_start;
    EXTERN clock_t throughput_clock;
    EXTERN pid_t pid;
    EXTERN pthread_mutex_t lock;
    EXTERN pthread_cond_t new_job; //marks if there is a new job
    EXTERN pthread_cond_t nfull;   //marks if queue is not full
    EXTERN pthread_cond_t nempty;  //marks if queue is not empty
    EXTERN pthread_cond_t full;    //marks if queue is full
    EXTERN pthread_cond_t empty;   //marks if queue is empty
    EXTERN pthread_cond_t test_complete;
    EXTERN pthread_cond_t succ_subm;       //marks a successful submission
    EXTERN struct Jobs queue[QUEUE_SIZE];  //shared job queue between scheduler and dispatcher
    EXTERN char g_job_name[25];    //global name for new jobs
    EXTERN int g_priority;    //global priority for new jobs
    EXTERN double g_cpu_time;  //global cpu time for new jobs
    EXTERN int jobs_queued;
    EXTERN int expected_wait;
    EXTERN int sort_type;      // 0 = fcfs, 1 = sjf, 2 = priority
    EXTERN int quit;           //signals threads to quit
    EXTERN int total_jobs_sub;     //total jobs submitted
    EXTERN double total_cpu_time;
    EXTERN struct timeval program_start_time;
    EXTERN double tot_turnaround;  //total turnaround time
    EXTERN double tot_wait_time;   //total wait time - since the policies are not preemptive, this is the same as total response time
    
    EXTERN int if_test;
    EXTERN double ave_cpu_time;
    EXTERN double ave_turn_time; 
    EXTERN double ave_wait_time;
    EXTERN double ave_throughput;

    EXTERN int test_task_num_total;
    EXTERN int test_task_num_disp;
    EXTERN pthread_mutex_t test_lock;

    EXTERN struct timeval t0;   //tracks current job start time, used in dispatcher and cmd line parser

#endif