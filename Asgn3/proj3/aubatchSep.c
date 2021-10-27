/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: aubatchSep.c (named because it is the separate compilation version)
    Function: provides scheduling functionality to aubatchSep.c 
    Relies on global.h, scheduler1.h, dispatcher.h, scheduler1.c, dispatcher.c
    Compile with Makefile by typing 'make' in command line
    March 13, 2021

    TODO: check on user input for decimals for test and run for max and min cpu time
*/

#define EXTERN 
#include "global.h"
#include "scheduler1.h"
#include "dispatcher.h"

#define QUEUE_SIZE 1000
    // mutex lock for job queue
    clock_t t;
    clock_t prog_start;
    clock_t throughput_clock;
    pid_t pid;
    pthread_mutex_t lock;
    pthread_mutex_t test_lock;
    pthread_cond_t new_job; //marks if there is a new job
    pthread_cond_t nfull;   //marks if queue is not full
    pthread_cond_t nempty;  //marks if queue is not empty
    pthread_cond_t full;    //marks if queue is full
    pthread_cond_t empty;   //marks if queue is empty
    pthread_cond_t test_complete;   //marks if test has finished its run
    pthread_cond_t succ_subm;       //marks a successful submission
    struct Jobs queue[QUEUE_SIZE];  //shared job queue between scheduler and dispatcher
    char g_job_name[25];    //global name for new jobs
    int g_priority = 10;    //global priority for new jobs
    double g_cpu_time = 10;    //global cpu time for new jobs
    int jobs_queued = 0;
    int expected_wait = 0;
    int sort_type = 0;      // 0 = fcfs, 1 = sjf, 2 = priority
    int quit = 0;           //signals threads to quit
    int total_jobs_sub = 0;     //total jobs submitted
    double total_cpu_time = 0;
    struct timeval program_start_time;
    double tot_turnaround = 0;  //total turnaround time
    double tot_wait_time = 0;   //total wait time - since the policies are not preemptive, this is the same as total response time

    // prepare some variables for statistics for 'test' command
    int if_test = 0;
    double ave_cpu_time = 0;
    double ave_turn_time = 0;
    double ave_wait_time = 0;
    double ave_throughput = 0;
    struct timeval t0;
    int test_task_num_disp = 0; //keeps track of how many tasks have come through the dispatcher per test
    int test_task_num_total = 0; //how many tasks we need from the dispatcher per test

// Function: main()
// Uses command line inputs
// Take user input
// Valid commands are 'help' 'run' 'list' 'sjf' 'fcfs' 'priority' 'test' and 'quit'
int main(int argc, char *argv[])
{  
    //----------------------------------
    // Variable Declaration
    //----------------------------------
    char command[255] = "";
    char user_input[10][10];
    pthread_t thread0;
    pthread_t thread1;
    int jobs_subm = 0;
    double turnaround = 0;
    double cpu_time = 0;
    double wait_time = 0;
    double throughput = 0;
    int threads_running = 0;

    struct timeval prog_start_time;
    gettimeofday(&prog_start_time, NULL);

    //begin scheduler and dispatcher
    if(pthread_create(&thread0, NULL, schedule_jobs, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    else if(pthread_create(&thread1, NULL, run_jobs, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    
    printf("Welcome to Aidan Lambrecht's batch job scheduler Version 1.0 \n \
        Type ‘help’ to find more about AUbatch commands.\n");
    fprintf(stderr, "> ");
    while (strcmp(command, "quit") != 0) {
        scanf("%[^\n]", &command);

        //--------------------------------------------------------
        // Could not split input by scanf(), only solution I could get 
        // to work was this janky thing. Based off:
        // https://www.w3resource.com/c-programming-exercises/string/c-string-exercise-31.php
        //--------------------------------------------------------
        //split input by space
        int j = 0; 
        int count = 0;
        for (int i = 0; i <= (strlen(command)); i++){
            if(command[i] == ' ' || command[i] == '\0') {
                user_input[count][j] = '\0';
                count = count + 1;  
                j = 0;    
            }
            else {
                user_input[count][j] = command[i];
                j = j + 1;
            }
        }
        //--------------------------------------------------------

        //----------------------------------------------------------------
        // Help
        //----------------------------------------------------------------
        if (strcmp(user_input[0], "help") == 0) {
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
        else if (strcmp(user_input[0], "run") == 0) {
            //get mutex to alter new job parameters
            pthread_mutex_lock(&lock);
            strcpy(g_job_name, user_input[1]);
            g_cpu_time = atof(user_input[2]);
            g_priority = atoi(user_input[3]);
            t = clock();
            pthread_mutex_unlock(&lock);
            pthread_cond_signal(&new_job);

            //calculate expected wait time by investigating queue
            wait_time = 0;
            if (jobs_queued != 0) {
                for (int i = 0; i < jobs_queued; i++) {
                    wait_time = wait_time + queue[i].cpu_time;
                }
                //factor in how long the current task has been running
                double start = ((double) t0.tv_sec + ((double) t0.tv_usec * 0.000001));
                struct timeval t1;
                gettimeofday(&t1, NULL);
                double now = ((double) t1.tv_sec + ((double) t1.tv_usec * 0.000001));
                double elapsed = now - start;
                wait_time = wait_time - elapsed;
            }

            printf("Job %s was submitted\n", user_input[1]);
            printf("Total number of jobs in the queue: %d\n", jobs_queued + 1);
            printf("Expected waiting time: %f seconds\n", wait_time);
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
        else if (strcmp(user_input[0], "list") == 0) {
            //acquire lock to check jobs queued
            pthread_mutex_lock(&lock);
            printf("Total number of jobs in the queue: %d\n", jobs_queued);
            if (sort_type == 0) {
                printf("Scheduling Policy: FCFS\n\n");
            }
            else if (sort_type == 1) {
                printf("Scheduling Policy: SJF\n\n");
            }
            else if (sort_type == 2) {
                printf("Scheduling Policy: Priority\n\n");
            }
            printf("Name    CPU_Time    Priority    Arrival_time    Progress\n");
            for (int i=0; i < jobs_queued; i++) {
                //find how long current job has been running by using current time - execution time
                if (i == 0) {
                    double t; 
                    t = queue[i].exec_time - queue[i].sub_time;
                    double time_taken_in_exec = ((double)t)/CLOCKS_PER_SEC;
                    double time_remaining = (double) queue[i].cpu_time - time_taken_in_exec;
                }
                printf("%-9s %-12.2f %-10d %d:%d:%d %13s\n", queue[i].job_name, queue[i].cpu_time, queue[i].priority, queue[i].hour, queue[i].min, queue[i].sec, queue[i].progress);
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
        else if (strcmp(user_input[0], "sjf") == 0) {
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
        else if (strcmp(user_input[0], "fcfs") == 0) {
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
        else if (strcmp(user_input[0], "priority") == 0) {
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
        else if (strcmp(user_input[0], "quit") == 0) {
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
                printf("\n");
            }
            struct timeval prog_end_time;
            gettimeofday(&prog_end_time, NULL);
            double time_sec = (prog_end_time.tv_sec - prog_start_time.tv_sec);
            double time_micro = ((double) prog_end_time.tv_usec - (double) prog_start_time.tv_usec) * 0.000001;
            double prog_runtime = time_sec + time_micro;
            printf("Total number of job submitted:  %d\n", total_jobs_sub);
            printf("Average turnaround time:        %.2f seconds\n", tot_turnaround / total_jobs_sub);
            printf("Average CPU time:               %.2f seconds\n", total_cpu_time / total_jobs_sub);
            printf("Average waiting time:           %.2f seconds\n", tot_wait_time / total_jobs_sub);
            printf("Throughput:                     %.4f No./second\n\n", total_jobs_sub / prog_runtime);  

            return 0;
        }

        //----------------------------------------------------------------
        // Test
        // test <benchmark> <policy> <num_of_jobs> <arrival_rate>
        //      <priority_levels> <min_CPU_time> <max_CPU_time>
        // arrival rate measured in # of jobs/second
        //----------------------------------------------------------------
        else if (strcmp(user_input[0], "test") == 0) {
            pthread_mutex_lock(&test_lock);
            if_test = 1;
            test_task_num_disp = 0;
            ave_cpu_time = 0;
            ave_turn_time = 0;
            ave_wait_time = 0;
            struct timeval test_start_time;
            gettimeofday(&test_start_time, NULL);            
            int flag = 0;
            while (jobs_queued != 0) {
                if (flag == 0) {
                    printf("Waiting for queue to empty before beginning %s. . .\n", user_input[1]);
                    flag = 1;
                }
            }
            printf("Queue is empty. Beginning benchmark %s. . .\n", user_input[1]);

            // set the policy
            if (strcmp(user_input[2], "fcfs") == 0) {
                sort_type = 0;
                fcfs_scheduler();   //need to call this here since it isn't called in the scheduler
            }
            else if (strcmp(user_input[2], "sjf") == 0) {
                sort_type = 1;
            }
            else if (strcmp(user_input[2], "priority") == 0) {
                sort_type = 2;
            }

            // make the number of jobs specified in the test command
            int num_jobs_for_test = atoi(user_input[3]);
            test_task_num_total = num_jobs_for_test;
            // get double from arrival rate parameter
            char *ptr;
            double arrival_rate;
            arrival_rate = strtod(user_input[4], &ptr);
            double arrival = arrival_rate * 100000;
            // get range of priority levels
            int priority_range = atoi(user_input[5]);
            // get minimum CPU time
            double min_cpu_time = atof(user_input[6]);
            // get maximum CPU time 
            double max_cpu_time = atof(user_input[7]);

            for (int i = 0; i < num_jobs_for_test; i++) {
                pthread_mutex_lock(&lock);
                strcpy(g_job_name, user_input[1]);
                //randomly assign priority and cpu time within range specified
                g_cpu_time = min_cpu_time + (rand() / (RAND_MAX/(max_cpu_time-min_cpu_time)));
                //ave_cpu_time = ave_cpu_time + g_cpu_time; //actually going to measure this in dispatcher
                g_priority = (rand() % (priority_range + 1));
                t = clock();
                pthread_cond_signal(&new_job);
                pthread_mutex_unlock(&lock);
                //wait before scheduling more threads according to the arrival rate
                usleep(arrival);
            }

            //wait for test to finish before allowing new input to commandline and before printing stats
            pthread_cond_wait(&test_complete, &test_lock);

            struct timeval test_end_time;
            gettimeofday(&test_end_time, NULL);
            double time_sec = (test_end_time.tv_sec - test_start_time.tv_sec);
            double time_micro = ((double) test_end_time.tv_usec - (double) test_start_time.tv_usec) * 0.000001;
            double test_runtime = time_sec + time_micro;

            printf("The total number of jobs submitted:  %d\n", num_jobs_for_test);
            printf("Average turnaround time:             %.2f seconds\n", ave_turn_time / num_jobs_for_test);
            printf("Average CPU time:                    %.2f seconds\n", ave_cpu_time / num_jobs_for_test);
            printf("Average waiting time:                %.2f seconds\n", ave_wait_time / num_jobs_for_test);
            printf("Throughput:                          %.4f No./second\n\n", (double) num_jobs_for_test / test_runtime);
            pthread_mutex_unlock(&lock);
            pthread_mutex_unlock(&test_lock);

            fprintf(stderr, "> ");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Workload
        // workload <policy>
        // arrival rate measured in # of jobs/second
        //----------------------------------------------------------------
        else if (strcmp(user_input[0], "workload") == 0) {
            int num_jobs_workload[] = {5, 10, 15, 20, 25};
            double arrival_rates_workload[] = {0.1, 0.2, 0.5, 1.0};
            int priority_workload[] = {5, 10, 15, 20, 25};
            double load_dist_min[] = {0.1, 0.1, 0.5, 1.0};
            double load_dist_max[] = {0.5, 1.0, 1.0, 10.0};
            FILE *fp;
            fp = fopen("Workload_Results_FCFS.txt", "a");
        
            for (int i = 0; i < 5; i++) {   //num_jobs
                for (int j = 0; j < 4; j++) {   //arrival_rates
                    for (int k = 0; k < 4; k++) {   //min, max job times
                        pthread_mutex_lock(&test_lock);
                        if_test = 1;
                        test_task_num_disp = 0;
                        ave_cpu_time = 0;
                        ave_turn_time = 0;
                        ave_wait_time = 0;
                        struct timeval test_start_time;
                        gettimeofday(&test_start_time, NULL);            
                        int flag = 0;
                        while (jobs_queued != 0) {
                            if (flag == 0) {
                                printf("Waiting for queue to empty before beginning workload condition testing. . .\n");
                                flag = 1;
                            }
                        }
                        printf("Queue is empty. Beginning benchmarks. . .\n");

                        // set the policy
                        if (strcmp(user_input[1], "fcfs") == 0) {
                            sort_type = 0;
                            fcfs_scheduler();   //need to call this here since it isn't called in the scheduler
                        }
                        else if (strcmp(user_input[1], "sjf") == 0) {
                            sort_type = 1;
                        }
                        else if (strcmp(user_input[1], "priority") == 0) {
                            sort_type = 2;
                        }

                        // make the number of jobs specified in the test command
                        int num_jobs_for_test = num_jobs_workload[i];
                        test_task_num_total = num_jobs_for_test;
                        // get double from arrival rate parameter
                        char *ptr;
                        double arrival_rate;
                        arrival_rate = arrival_rates_workload[j];
                        double arrival = arrival_rate * 100000;
                        // get range of priority levels
                        int priority_range = priority_workload[i];
                        // get minimum CPU time
                        double min_cpu_time = load_dist_min[k];
                        // get maximum CPU time 
                        double max_cpu_time = load_dist_max[k];

                        for (int i = 0; i < num_jobs_for_test; i++) {
                            pthread_mutex_lock(&lock);
                            strcpy(g_job_name, "job1");
                            //randomly assign priority and cpu time within range specified
                            g_cpu_time = min_cpu_time + (rand() / (RAND_MAX/(max_cpu_time-min_cpu_time)));
                            g_priority = (rand() % (priority_range + 1));
                            t = clock();
                            pthread_mutex_unlock(&lock);
                            pthread_cond_signal(&new_job);
                            //wait before scheduling more threads according to the arrival rate
                            usleep(arrival);
                        }
                        printf("Workload Conditions for test: \n");
                        printf("Priority Range:                      0-%d\n", priority_range);
                        printf("Minimum CPU time:                    %f\n", min_cpu_time);
                        printf("Maximum CPU time:                    %f\n", max_cpu_time);
                        printf("Arrival rates:                       %f per second\n", arrival_rate);
                        printf("The total number of jobs submitted:  %d\n", num_jobs_for_test);
                        fprintf(fp, "Workload Conditions for test: \n");
                        fprintf(fp, "Priority Range:                      0-%d\n", priority_range);
                        fprintf(fp, "Minimum CPU time:                    %f\n", min_cpu_time);
                        fprintf(fp, "Maximum CPU time:                    %f\n", max_cpu_time);
                        fprintf(fp, "Arrival rates:                       %f per second\n", arrival_rate);
                        fprintf(fp, "The total number of jobs submitted:  %d\n", num_jobs_for_test);

                        //wait for test to finish before allowing new input to commandline and before printing stats
                        pthread_cond_wait(&test_complete, &test_lock);

                        struct timeval test_end_time;
                        gettimeofday(&test_end_time, NULL);
                        double time_sec = (test_end_time.tv_sec - test_start_time.tv_sec);
                        double time_micro = ((double) test_end_time.tv_usec - (double) test_start_time.tv_usec) * 0.000001;
                        double test_runtime = time_sec + time_micro;

                        printf("Average turnaround time:             %.2f seconds\n", ave_turn_time / num_jobs_for_test);
                        printf("Average CPU time:                    %.2f seconds\n", ave_cpu_time / num_jobs_for_test);
                        printf("Average waiting time:                %.2f seconds\n", ave_wait_time / num_jobs_for_test);
                        printf("Throughput:                          %.4f No./second\n\n", (double) num_jobs_for_test / test_runtime);
                        fprintf(fp, "Average turnaround time:             %.2f seconds\n", ave_turn_time / num_jobs_for_test);
                        fprintf(fp, "Average CPU time:                    %.2f seconds\n", ave_cpu_time / num_jobs_for_test);
                        fprintf(fp, "Average waiting time:                %.2f seconds\n", ave_wait_time / num_jobs_for_test);
                        fprintf(fp, "Throughput:                          %.4f No./second\n\n", (double) num_jobs_for_test / test_runtime);
                        pthread_mutex_unlock(&lock);
                        pthread_mutex_unlock(&test_lock);
                    }
                }
            }
            fclose(fp);
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