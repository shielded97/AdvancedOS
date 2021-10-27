// Advanced Operating Systems
// Aidan Lambrecht
// Objective:
//  Write AU Batch program
//  1) Scheduler: Producer
//  2) Dispatcher: Consumer
//  3) Implement FCFS, SJF, Priority
//  4) Implement jobs


//#include <cstring>
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 

#define QUEUE_SIZE 20

// Critical variables and mutex locks
//  Job array, mutex locks, condition variables?
    // Create job struct
    struct Jobs {
        char job_name[25];
        int cpu_time;
        int priority;
        double arrival_time;
        char progress[25];
    };  
    // mutex lock for job queue
    pthread_mutex_t lock;
    pthread_cond_t nfull;    //marks if queue is not full
    pthread_cond_t nempty;   //marks if queue is not empty
    pthread_cond_t full;    //marks if queue is full
    pthread_cond_t empty;   //marks if queue is empty
    struct Jobs queue[QUEUE_SIZE];  //shared job queue between scheduler and dispatcher
    struct Jobs cqueue[(2 * QUEUE_SIZE)];  //circular array inst.
    int jobs_queued = 0;
    int expected_wait = 0;

// Function: remove_job()
//  removes the first job from the queue after it is run
//  ONLY CALL THIS AFTER A MUTEX IS ACQUIRED
void remove_job() {
    int i;
    // move every job up in the queue by one spot
    for (i = 0; i < QUEUE_SIZE-1; i++) {
        queue[i] = queue[i+1];
    }

    // replace the last job with an empty job
    struct Jobs job;
    queue[i] = job;

    // and update the circular array queue also
    for (int i = 0; i < QUEUE_SIZE; i++) {
        cqueue[i] = queue[i];
        cqueue[i + QUEUE_SIZE] = queue[i];
    }

    return NULL;
} 

// Function: dispatcher()
//  for new threads
void *parser(void *param) {
    printf("Hi from thread 3.\n");
    char *args[]={"./parser",NULL}; 
    execv(args[0],args); 
    return NULL;
}

void make_thread_p() {
    pthread_t thread0;

    if(pthread_create(&thread0, NULL, parser, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    
    if (pthread_join(thread0, NULL)) {
        printf(stderr, "Error joining thread\n");
        return 1;
    }
}

// Function: dispatcher()
//  for new threads
void *dispatcher(void *param) {
    printf("Hi from thread 2.\n");
    char *args[]={"./process",NULL}; 
    execv(args[0],args); 
    return NULL;
}

void make_thread_d() {
    pthread_t thread0;
    if(pthread_create(&thread0, NULL, dispatcher, 0)) {
        printf(stderr, "Error creating thread\n");
        return 1;
    }
    
    if (pthread_join(thread0, NULL)) {
        printf(stderr, "Error joining thread\n");
        return 1;
    }
}

//TODO implement different scheduling tools
// Function: schedule_jobs()
void schedule_jobs(name, time, priority) {
    //create new job
    struct Jobs temp;
    temp.priority = (int) priority;
    temp.cpu_time = (int) time;
    strcpy(temp.job_name, &name);
    //feed to job queue    
    //  acquire lock
    pthread_mutex_lock(&lock);
    //  wait until queue is not full
    while (jobs_queued == QUEUE_SIZE) {
        pthread_cond_wait(&nfull, &lock);
    }
    //  add new job to end of queue
    queue[jobs_queued] = temp;

    //  copy new queue into circular array
    //  every time we add a job, copy whole queue
    for (int i = 0; i < QUEUE_SIZE; i++) {
        cqueue[i] = queue[i];
        cqueue[i + QUEUE_SIZE] = queue[i];
    }

    jobs_queued++;
    
    if (jobs_queued == QUEUE_SIZE) {
        pthread_cond_signal(&full);
    }
    //  signal that queue is not empty
    pthread_cond_signal(&nempty);
    //unlock mutex for future
    pthread_mutex_unlock(&lock);
}

// Function: run_jobs()
//  Take the job queue and execute the first process in the list
//  Remove the job from the queue and move every other process up
void run_jobs() {
    //acquire lock
    pthread_mutex_lock(&lock);
    //check if queue is empty
    while (jobs_queued == 0) {
        pthread_cond_wait(&nempty, &lock);
    }
    //if not empty, take first job and execute
    struct Jobs job = queue[0];
    // char *args[]={"./process",NULL}; 
    // execv(args[0],args); 

    //now exorcise the job from the queue
    remove_job();
    jobs_queued--;

    //if empty, signal so no more runs
    if (jobs_queued == 0) {
         pthread_cond_signal(&empty);
    }
    // signal that queue is not full
    pthread_cond_signal(&nfull);
    //unlock mutex for future
    pthread_mutex_unlock(&lock);
}

// Function: main()
//  Uses command line inputs
// Take user input
// Valid commands are 'help' 'run' 'list' and 'change' (scheduling policies)
//
int main( int argc, char *argv[])
{  
    
    //----------------------------------
    // Variable Declaration
    //----------------------------------
    char command[255] = "";
    char newString[10][10];
    pthread_t thread0;
    char policy[] = "sjf";
    int jobs_subm = 0;
    double turnaround = 0;
    double cpu_time = 0;
    double wait_time = 0;
    double throughput = 0;

    //Run dispatcher in second thread
    //make_thread_d();
    //Keep scheduler in this one?
    
    printf("Welcome to Aidan Lambrecht's batch job scheduler Version 1.0 \n \
        Type ‘help’ to find more about AUbatch commands.\n");
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
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Run
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "run") == 0) {
            printf("Job %s was submitted\n", newString[1]);
            printf("Total number of jobs in the queue: %d\n", 4);
            printf("Expected waiting time: %d seconds\n", 113);
            printf("Scheduling Policy: %s\n\n", "FCFS");
            
            schedule_jobs(newString[1], newString[2], newString[3]);

            //get new input
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // List
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "list") == 0) {
            printf("Total number of jobs in the queue: %d\n", 4);
            printf("Scheduling Policy: %s\n\n", "FCFS");
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // SJF
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "sjf") == 0) {
            printf("Scheduling policy is switched to %s.\n", "SJF");
            printf("All the %d waiting jobs have been rescheduled.\n\n", 3);
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // FCFS
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "fcfs") == 0) {
            printf("Scheduling policy is switched to %s.\n", "FCFS");
            printf("All the %d waiting jobs have been rescheduled.\n\n", 3);
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Priority 
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "priority") == 0) {
            printf("Scheduling policy is switched to %s.\n", "Priority");
            printf("All the %d waiting jobs have been rescheduled.\n\n", 3);
            fgets(&command, 255, stdin);
        }

        //----------------------------------------------------------------
        // Quit
        //----------------------------------------------------------------
        else if (strcmp(newString[0], "quit") == 0) {
            printf("Total number of job submitted: %d\n", 5);
            printf("Average turnaround time: %d seconds\n", 5);
            printf("Average CPU time: %d seconds\n", 5);
            printf("Average waiting time: %d seconds\n", 5);
            printf("Throughput: %d No./second\n\n", 5);        
            return 0;
        }

        //----------------------------------------------------------------
        // Invalid Input
        //----------------------------------------------------------------
        else {
            printf("\nType ‘help’ to find more about AUbatch commands.\n\n");
            fgets(&command, 255, stdin);
        }
    }
    return 0;
}