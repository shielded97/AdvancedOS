/*
    Author: Aidan Lambrecht
    Class: COMP 7500
    Name: dispatcher.h
    Function: header file for dispatcher.c, defining remove_job() and run_jobs() 
    March 13, 2021
*/

#ifndef DISPATCHER_H_INCLUDED
#define DISPATCHER_H_INCLUDED

// Function: remove_job()
//  removes the first job from the queue after it is run
//  ONLY CALL THIS AFTER A MUTEX IS ACQUIRED
void remove_job();

// Function: run_jobs()
//  Take the job queue and execute the first process in the list
//  Remove the job from the queue and move every other process up
void *run_jobs(void *param);

#endif