// Command line parser for au batch

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h>
#include <pthread.h> 

void schedule_job(name, time, priority) {
    
    
    return NULL;
}

int main() {

    char command[255] = "";
    char newString[10][10];

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
            
            schedule_job(newString[1], newString[2], newString[3]);

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