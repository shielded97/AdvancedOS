// Advanced Operating Systems
// Aidan Lambrecht
// Objective:
//  Write AU Batch program
//  KNOWN ISSUES:
//      - No benchmarks or evals yet
//      - need to implement 'test'
//      - fix naive throughput
//      - dispatcher thread doesn't cancel correctly

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h>
#include <time.h> 

#define QUEUE_SIZE 1000

// Critical variables and mutex locks
//  Job array, mutex locks, condition variables?
    // Create job struct
    struct Jobs {
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
    clock_t t;
    pid_t pid;
    pthread_mutex_t lock;
    pthread_cond_t new_job; //marks if there is a new job
    pthread_cond_t nfull;   //marks if queue is not full
    pthread_cond_t nempty;  //marks if queue is not empty
    pthread_cond_t full;    //marks if queue is full
    pthread_cond_t empty;   //marks if queue is empty
    struct Jobs queue[QUEUE_SIZE];  //shared job queue between scheduler and dispatcher
    char g_job_name[25];    //global name for new jobs
    int g_priority = 10;    //global priority for new jobs
    int g_cpu_time = 10;    //global cpu time for new jobs
    int jobs_queued = 0;
    int expected_wait = 0;
    int sort_type = 0;      // 0 = fcfs, 1 = sjf, 2 = priority
    int quit = 0;           //signals threads to quit
    int total_jobs_sub = 0;     //total jobs submitted
    int total_cpu_time = 0;
    struct timeval program_start_time;
    double tot_turnaround = 0;  //total turnaround time
    double tot_wait_time = 0;   //total wait time - since the policies are not preemptive, this is the same as total response time


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

    return NULL;
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

        // prepare the job for running
        strcpy(name, "./");
        strcat(name, job.job_name);
        char *args[]={name, NULL};

        // execute scheduled job
        // first fork a new process
        switch ((pid = fork())) {
            case -1: //failure
                printf("failed to fork to execv command\n");
            case 0: ;//child executes execv 
                t = clock();
                queue[0].exec_time = t;
                execv(args[0],args); 
            default: //parent takes stats
                //wait to remove job until it finishes executing
                printf("Job %s is executing. . .\n", queue[0].job_name);
                pthread_mutex_unlock(&lock);    //unlock mutex for future
                sleep(queue[0].cpu_time);
                struct timeval t1;
                gettimeofday(&t1, NULL);
                double turnaround = t1.tv_sec - queue[0].sub_time_sec;
                double wait_time = turnaround - queue[0].cpu_time;
                tot_turnaround = tot_turnaround + turnaround;  //add this job's turnaround to total turnaround
                tot_wait_time = tot_wait_time + wait_time;
                pthread_mutex_lock(&lock);  //acquire lock
                printf("Job %s is finished\n", queue[0].job_name);
        } 
        
        remove_job();   //now exorcise the job from the queue

        if (jobs_queued == 0) { //if empty, signal so no more runs
             pthread_cond_signal(&empty);
        }
        pthread_cond_signal(&nfull);    // signal that queue is not full
        pthread_mutex_unlock(&lock);    //unlock mutex for future
    }
}


// Function: main()
//  Uses command line inputs
// Take user input
// Valid commands are 'help' 'run' 'list' 'sjf' 'fcfs' 'priority' 'test' and 'quit'
int main( int argc, char *argv[])
{  
    //----------------------------------
    // Variable Declaration
    //----------------------------------
    char command[255] = "";
    char newString[10][10];
    pthread_t thread0;
    pthread_t thread1;
    int jobs_subm = 0;
    double turnaround = 0;
    double cpu_time = 0;
    double wait_time = 0;
    double throughput = 0;
    int threads_running = 0;

    gettimeofday(&program_start_time, NULL);    //get start time in seconds

    //begin scheduler and dispatcher
    if(pthread_create(&thread0, NULL, schedule_jobs, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    else if(pthread_create(&thread1, NULL, run_jobs, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    else {
        printf("Created scheduler and dispatcher\n");
    }
    
    printf("Welcome to Aidan Lambrecht's batch job scheduler Version 1.0 \n \
        Type ‘help’ to find more about AUbatch commands.\n");
    fprintf(stderr, "> ");
    while (strcmp(command, "quit") != 0) {
        scanf("%[^\n]", &command);

        //--------------------------------------------------------
        // Gotta change this up a bit
        //--------------------------------------------------------
        //split input by space
        int j=0; 
        int ctr=0;
        for(int i=0;i<=(strlen(command));i++)
        {
            // if space or NULL found, assign NULL into newString[ctr]
            if(command[i]==' '||command[i]=='\0')
            {
                newString[ctr][j]='\0';
                ctr++;  //for next word
                j=0;    //for next word, init index to 0
            }
            else
            {
                newString[ctr][j]=command[i];
                j++;
            }
        }
        //--------------------------------------------------------

        //----------------------------------------------------------------
        // Help
        //----------------------------------------------------------------
        if (strcmp(newString[0], "help") == 0) {
            printf("run <job> <time> <pri>: submit a job named <job>,\n \
                execution time is <time>,\n \
                priority is <pri>.\n \
                list: display the job status.\n \
                fcfs: change the scheduling policy to FCFS.\n \
                sjf: change the scheduling policy to SJF.\n \
                priority: change the scheduling policy to priority.\n \
                test <benchmark> <policy> <num_of_jobs> <priority_levels>\n \
                <min_CPU_time> <max_CPU_time>\n \
                quit: exit AUbatch\n\n");
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Run
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "run") == 0) {
            //get mutex to alter new job parameters
            pthread_mutex_lock(&lock);
            strcpy(g_job_name, newString[1]);
            g_cpu_time = atoi(newString[2]);
            g_priority = atoi(newString[3]);
            t = clock();
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&new_job);

            printf("Job %s was submitted\n", newString[1]);
            printf("Total number of jobs in the queue: %d\n", jobs_queued);
            printf("Expected waiting time: %d seconds\n", wait_time);
            if (sort_type == 0){
                printf("Scheduling Policy: FCFS\n\n");
            }
            else if (sort_type == 1) {
                printf("Scheduling Policy: SJF\n\n");
            }
            else if (sort_type == 2) {
                printf("Scheduling Policy: Priority\n\n");
            }
            fprintf(stderr, "> ");
            //get new input
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // List
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "list") == 0) {
            //acquire lock to check jobs queued
            pthread_mutex_lock(&lock);
            printf("Total number of jobs in the queue: %d\n", jobs_queued);
            if (sort_type == 0){
                printf("Scheduling Policy: FCFS\n\n");
            }
            else if (sort_type == 1) {
                printf("Scheduling Policy: SJF\n\n");
            }
            else if (sort_type == 2) {
                printf("Scheduling Policy: Priority\n\n");
            }
            printf("Name CPU_Time Pri Arrival_time Progress\n");
            for (int i=0; i < jobs_queued; i++) {
                //find how long current job has been running by using current time - execution time
                if (i == 0) {
                    clock_t t; 
                    t = queue[i].exec_time - queue[i].sub_time;
                    double time_taken_in_exec = ((double)t)/CLOCKS_PER_SEC;
                    double time_remaining = (double) queue[i].cpu_time - time_taken_in_exec;
                }
                printf("%s %d %d %d:%d:%d %s\n", queue[i].job_name, queue[i].cpu_time, queue[i].priority, queue[i].hour, queue[i].min, queue[i].sec, queue[i].progress);
            }
            printf("\n");

            //release thread
            pthread_mutex_unlock(&lock);
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // SJF
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "sjf") == 0) {
            pthread_mutex_lock(&lock);
            sort_type = 1;
            sjf_scheduler();
            pthread_mutex_unlock(&lock);
            printf("Scheduling policy is switched to %s.\n", "SJF");
            printf("All the %d waiting jobs have been rescheduled.\n\n", jobs_queued);
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // FCFS
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "fcfs") == 0) {
            pthread_mutex_lock(&lock);
            sort_type = 0;
            fcfs_scheduler();
            pthread_mutex_unlock(&lock);
            printf("Scheduling policy is switched to %s.\n", "FCFS");
            printf("All the %d waiting jobs have been rescheduled.\n\n", jobs_queued);
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Priority 
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "priority") == 0) {
            pthread_mutex_lock(&lock);
            sort_type = 2;
            pri_scheduler();
            pthread_mutex_unlock(&lock);
            printf("Scheduling policy is switched to %s.\n", "Priority");
            printf("All the %d waiting jobs have been rescheduled.\n\n", jobs_queued);
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Quit
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "quit") == 0) {
            pthread_mutex_lock(&lock);
            quit = 1;
            pthread_mutex_unlock(&lock);
            if (pthread_cancel(thread0)) {
                printf(stderr, "Error joining scheduler thread\n");
                return NULL;
            }
            else if (pthread_cancel(thread1)) {
                printf(stderr, "Error joining dispatcher thread\n");
                return NULL;
            }
            else {
                printf("Scheduler and dispatcher successfully joined\n");
            }
            struct timeval end_time;
            gettimeofday(&end_time, NULL);
            double runtime = end_time.tv_sec - program_start_time.tv_sec;
            printf("Total number of job submitted: %d\n", total_jobs_sub);
            printf("Average turnaround time: %d seconds\n", tot_turnaround / total_jobs_sub);
            printf("Average CPU time: %d seconds\n", total_cpu_time / total_jobs_sub);
            printf("Average waiting time: %d seconds\n", tot_wait_time / total_jobs_sub);
            printf("Throughput: %d No./second\n\n", runtime / total_jobs_sub);  // very naive implementation of throughput 
            // Should throughput be calculated only when the program has jobs in the queue or also while it's waiting for job submissions?

            return 0;
        }

        //----------------------------------------------------------------
        // Test
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "test") == 0) {
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Invalid Input
        //----------------------------------------------------------------
        else {
            printf("\nType ‘help’ to find more about AUbatch commands.\n\n");
            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }
    }
    return 0;
}