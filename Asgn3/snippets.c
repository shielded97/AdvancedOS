//         // if input is not 'help', try to split input by spaces
//         // strcpy(copies, command);
//         //char* comm;        
//         // char * comm = strtok(copies, " ");
//         // while (comm != NULL){
//         //     printf("%s\n", comm);
//         //     comm = strtok(NULL, " ");
//         //}

        
//         for(int i=0;i < ctr;i++)
//             printf(" %s\n",newString[i]);

//     // NOTE: I think this is unecessary because if we move all
//     // other jobs up in the queue and reduce the jobs_queued 
//     // variable, then the scheduler will just overwrite that one.

//     // replace the last job with an empty job
//     // struct Jobs job;
//     // queue[i] = job;

//     // and update the circular array queue also
//     // for (int i = 0; i < QUEUE_SIZE; i++) {
//     //     cqueue[i] = queue[i];
//     //     cqueue[i + QUEUE_SIZE] = queue[i];
//     // }

//     // printf("job_name: ");
//     // printf(temp.job_name);
//     // // printf("*job_name: ");
//     // // printf(*temp.job_name);
//     // printf("\n&job_name: ");
//     // printf(&temp.job_name);
//     // printf("\n");
//     // struct Jobs *temp1;
//     // strcpy(temp1->job_name, *name);
//     // temp1->priority = priority;

// void make_thread_p(char *name, int time, int priority, pthread_t thread0) {
//     struct Jobs temp;
//     strcpy(temp.job_name, "sample");
//     temp.priority = priority;
//     temp.cpu_time = time;

//     if(pthread_create(&thread0, NULL, schedule_jobs, &temp)) {
//         printf(stderr, "Error creating thread\n");
//         return 1;
//     }
// }

//     printf(temp->job_name);
//     printf("\n");

//     struct Jobs job;
//     strcpy(job.job_name, temp->job_name);
//     job.priority = temp->priority;
//     job.min = temp->min;
//     job.sec = temp->sec;
//     job.hour = temp->hour;

//     // // Function: join_all_threads()
// // //  joins all threads when closing application
// // void join_all_threads(pthread_t thread0, pthread_t thread1, int threads_running) {
// //     if (threads_running == 1) {
//         // if (pthread_join(thread0, NULL)) {
//         //     printf(stderr, "Error joining scheduler thread\n");
//         //     return NULL;
//         // }
// //         if (pthread_join(thread1, NULL)) {
// //             printf(stderr, "Error joining dispatcher thread\n");
// //             return NULL;
// //         }
// //     }
// // }

// // Function: make_thread_d()
// // //  runs the dispatcher thread
// // void make_thread_d(pthread_t thread0) {
// //     if(pthread_create(&thread0, NULL, run_jobs, 0)) {
// //         printf(stderr, "Error creating thread\n");
// //         return 1;
// //     }
// // }

// // // Function: make_thread_p()
// // //  runs the scheduler thread
// // void make_thread_p(pthread_t thread0) {
// //     if(pthread_create(&thread0, NULL, schedule_jobs, 0)) {
// //         printf(stderr, "Error creating thread\n");
// //         return 1;
// //     }
// // }

            // clock_t test_end = clock();
            // clock_gettime(CLOCK_REALTIME, &end);
            // double time_spent = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1000000000;
            //printf("Throughput with clock_gettime(): %f No./second\n\n", (double) num_jobs_for_test / time_spent);

        // struct timespec start;
        // clock_gettime(CLOCK_REALTIME, &start);
        //temp.sub_time = //(start.tv_sec + start.tv_nsec) / 1000000000;//(double) t  + (double) jobs_queued;

            //double test_start = (double) test_start_time.tv_sec + ((double) test_start_time.tv_usec * 0.000001);
            //clock_t test_start = clock();
            // struct timespec start, end;
            // clock_gettime(CLOCK_REALTIME, &start);
                        // struct timeval end_time;
            // gettimeofday(&end_time, NULL);
            // double runtime = end_time.tv_sec - program_start_time.tv_sec;
                        //double prog_end = prog_end_time.tv_sec + prog_end_time.tv_usec;
            //throughput_clock = clock();
                //printf("Scheduler and dispatcher successfully joined\n");
            //pthread_cond_wait(&succ_subm, &lock);
    //gettimeofday(&program_start_time, NULL);    //get start time in seconds
    //prog_start = clock();