/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: dispatcher.c
    Function: provides dispatching functionality to aubatchSep.c 
    Relies on dispatcher.h, global.h, and aubatchSep.c
    March 13, 2021
*/

#include "global.h"
#include "dispatcher.h"
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <time.h> 
#include<sys/wait.h>

#define QUEUE_SIZE 1000

// Function: remove_job()
//  removes the first job from the queue after it is run
//  ONLY CALL THIS AFTER A MUTEX IS ACQUIRED
void remove_job() {
    int i;
    // move every job up in the queue by one spot
    for (i = 0; i < QUEUE_SIZE-1; i++) {
        queue[i] = queue[i+1];
    }

    jobs_queued--;
} 

// Function: run_jobs()
//  Take the job queue and execute the first process in the list
//  Remove the job from the queue and move every other process up
void *run_jobs(void *param) {
    while (quit == 0) {
        pthread_mutex_lock(&lock);  //acquire lock
        while (jobs_queued == 0) {  //check if queue is empty
            pthread_cond_wait(&nempty, &lock);
        }
        struct Jobs job = queue[0]; //if not empty, take first job and execute
        strcpy(queue[0].progress, "run");
        char name[40];
        char exec_time[5];

        // prepare the job for running
        sprintf(exec_time, "%f", job.cpu_time);
        strcpy(name, "./");
        strcat(name, job.job_name);
        char *args[]={name, exec_time, NULL};

        // execute scheduled job
        // record start time of execution
        gettimeofday(&t0, NULL);
        queue[0].exec_time = ((double) t0.tv_sec + ((double) t0.tv_usec * 0.000001));
        // first fork a new process
        switch ((pid = fork())) {
            case -1: //failure
                printf("failed to fork to execv command\n");
            case 0: ;//child executes execv 
                execv(args[0],args); 
            default: ;//parent takes stats
                //wait to remove job until it finishes executing
                pthread_mutex_unlock(&lock);    //unlock mutex for future
                int status;
                double turnaround = 0;
                double wait_time = 0;
                double burst_time = 0;
                wait(&status);  // wait for forked process to complete
                struct timeval t1;
                gettimeofday(&t1, NULL);

                // take a bunch of statistics
                turnaround = ((double)t1.tv_sec + ((double) t1.tv_usec * 0.000001) - queue[0].sub_time);
                burst_time = (((double)t1.tv_sec + ((double) t1.tv_usec * 0.000001)) - queue[0].exec_time);
                wait_time = turnaround - burst_time;
                tot_turnaround = tot_turnaround + turnaround;  //add this job's turnaround to total turnaround
                tot_wait_time = tot_wait_time + wait_time;
                total_cpu_time = total_cpu_time + burst_time;
                ave_cpu_time = ave_cpu_time + burst_time;
                ave_turn_time = ave_turn_time + turnaround;
                ave_wait_time = ave_wait_time + wait_time;
                pthread_mutex_lock(&lock);  //acquire lock
        } 
        
        remove_job();   //now exorcise the job from the queue

        if (if_test == 1) {
            test_task_num_disp++;
        }

        if (jobs_queued == 0) { //if empty, signal so no more runs
            // if test is complete, signal to print statistics
            if (if_test == 1 && test_task_num_disp == test_task_num_total) {
                if_test == 0;
                pthread_mutex_lock(&test_lock);
                pthread_cond_signal(&test_complete);
                pthread_mutex_unlock(&test_lock);
            }
            pthread_cond_signal(&empty);
        }
        pthread_cond_signal(&nfull);    // signal that queue is not full
        pthread_mutex_unlock(&lock);    //unlock mutex for future
    }
}
 