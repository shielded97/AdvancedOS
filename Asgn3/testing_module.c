/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: testing_module.c
    Function: provides testing functionality to aubatchSep.c 
    Relies on global.h, testing_module.h
    March 13, 2021
*/

void workload_test() {
    int num_jobs[] = {5, 10, 15, 20, 25};
    double arrival_rates[] = {0.1, 0.2, 0.5, 1.0};
    double load_dist_min[] = {0.1, 0.1, 0.5, 1.0};
    double load_dist_max[] = {0.5, 1.0, 1.0, 10.0};

    for (int i = 0; i < 5; i++) {   //num_jobs
        for (int j = 0; j < 4; j++) {   //arrival_rates
            for (int k = 0; k < 4; k++) {   //min, max job times
                // fork then execv with these parameters?
                
            }
        }
    }
}