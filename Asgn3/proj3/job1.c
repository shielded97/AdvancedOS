/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: job1.c
    Function: dummy job to be run by AubatchSep.c
    Configurable runtime for 'test' command 
    March 13, 2021

    'test command format'
        // test <benchmark> <policy> <num_of_jobs> <arrival_rate>
        //      <priority_levels> <min_CPU_time> <max_CPU_time>

    // run this in batch with aubatch via 'test job1 sjf 5 1 5 1 5'
    // test job1 sjf 1 1 1 0.1 0.1

*/

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// takes input from the calling execv() function to decide how long to execute 
int main(int argc, char *argv[]) {
    int sleep_time = atoi(argv[1]);
    sleep(sleep_time);
    printf("%s finished running. . .\n", argv[0]);
    exit(1);
    return 1;
}