// job2.c 
// a dummy job that aubatch can execute for benchmark purposes

// test <benchmark> <policy> <num_of_jobs> <arrival_rate>
//      <priority_levels> <min_CPU_time> <max_CPU_time>

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

// run this in batch with aubatch via 'test job2 sjf 2 1 1 1 1'

int main(int argc, char *argv[]) {
    int run_time = atoi(argv[1]) * 1000000;
    // for (int i = 0; i < run_time; i++) {
    //     int k = i + run_time;
    //     // for (int j = 0; j < run_time; j++) {
    //     //     int l = j + k;
    //     // }
    // }
    printf("%s finished running. . .\n", argv[0]);
    exit(1);
    return 1;
}