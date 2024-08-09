#define _GNU_SOURCE  // Necessary for sched_getcpu
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <time.h>

// Constants for course and assignment identification
#define COURSE_NUMBER 2
#define ASSIGNMENT_NUMBER 3

// Define the Least Common Multiple (LCM) period in seconds
#define LCM_PERIOD 20  

// Function to simulate computational workload using sleep
void perform_computation(int computation_time_ms) {
    usleep(computation_time_ms * 1000);  // Convert milliseconds to microseconds
}

// Function to log the start of a thread with system information
void log_thread_start(int thread_id) {
    struct utsname uname_data;
    uname(&uname_data);  // Get system information

    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    char time_str[100];
    strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", t);  // Format time as a string

    int core_id = sched_getcpu();  // Get the CPU core the thread is running on

    // Log the start of the thread with system information
    syslog(LOG_INFO, "%s %s [COURSE:%d][ASSIGNMENT:%d]: Thread %d start @ %s on core %d",
           uname_data.sysname, uname_data.nodename, COURSE_NUMBER, ASSIGNMENT_NUMBER, thread_id, time_str, core_id);
}

// Function to execute thread S1's workload
void *thread_s1(void *threadid) {
    int thread_id = *(int *)threadid;
    int period = 2;  // Period in seconds
    int computation_time = 1;  // Computation time in milliseconds

    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);  // Get the current time
    time_t end_time = time(NULL) + LCM_PERIOD;  // Define the end time for the thread

    while (time(NULL) < end_time) {
        log_thread_start(thread_id);
        perform_computation(computation_time);
        next_activation.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL);  // Sleep until the next activation
    }
    pthread_exit(NULL);
}

// Function to execute thread S2's workload
void *thread_s2(void *threadid) {
    int thread_id = *(int *)threadid;
    int period = 5;  // Period in seconds
    int computation_time = 1;  // Computation time in milliseconds

    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);  // Get the current time
    time_t end_time = time(NULL) + LCM_PERIOD;  // Define the end time for the thread

    while (time(NULL) < end_time) {
        log_thread_start(thread_id);
        perform_computation(computation_time);
        next_activation.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL);  // Sleep until the next activation
    }
    pthread_exit(NULL);
}

// Function to execute thread S3's workload
void *thread_s3(void *threadid) {
    int thread_id = *(int *)threadid;
    int period = 10;  // Period in seconds
    int computation_time = 2;  // Computation time in milliseconds

    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);  // Get the current time
    time_t end_time = time(NULL) + LCM_PERIOD;  // Define the end time for the thread

    while (time(NULL) < end_time) {
        log_thread_start(thread_id);
        perform_computation(computation_time);
        next_activation.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL);  // Sleep until the next activation
    }
    pthread_exit(NULL);
}

// Function to execute thread S4's workload
void *thread_s4(void *threadid) {
    int thread_id = *(int *)threadid;
    int period = 20;  // Period in seconds
    int computation_time = 2;  // Computation time in milliseconds

    struct timespec next_activation;
    clock_gettime(CLOCK_MONOTONIC, &next_activation);  // Get the current time
    time_t end_time = time(NULL) + LCM_PERIOD;  // Define the end time for the thread

    while (time(NULL) < end_time) {
        log_thread_start(thread_id);
        perform_computation(computation_time);
        next_activation.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_activation, NULL);  // Sleep until the next activation
    }
    pthread_exit(NULL);
}

// Function to log system information using the uname command
void log_uname() {
    FILE *fp;
    char buffer[256];

    fp = popen("uname -a", "r");
    if (fp == NULL) {
        syslog(LOG_ERR, "[COURSE:%d][ASSIGNMENT:%d] Failed to run uname command", COURSE_NUMBER, ASSIGNMENT_NUMBER);
        return;  // Graceful return on failure
    }

    // Read the output line by line and log it
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        syslog(LOG_INFO, "[COURSE:%d][ASSIGNMENT:%d] %s", COURSE_NUMBER, ASSIGNMENT_NUMBER, buffer);
    }

    pclose(fp);
}

// Main function to set up and run the threads
int main() {
    pthread_t threads[4];
    int thread_args[4];
    int rc;

    // Initialize syslog for logging
    openlog("fibonacci_syslog", LOG_PID | LOG_CONS, LOG_USER);

    // Log system information using uname
    log_uname();

    // Set up attributes for real-time scheduling (FIFO policy)
    pthread_attr_t attr;
    struct sched_param param;

    pthread_attr_init(&attr);
    pthread_attr_setschedpolicy(&attr, SCHED_FIFO);

    // Create and start thread S1 with FIFO priority 10
    thread_args[0] = 1;
    param.sched_priority = 10;
    pthread_attr_setschedparam(&attr, &param);
    rc = pthread_create(&threads[0], &attr, thread_s1, (void *)&thread_args[0]);
    if (rc) {
        syslog(LOG_ERR, "[COURSE:%d][ASSIGNMENT:%d] ERROR; return code from pthread_create() is %d", COURSE_NUMBER, ASSIGNMENT_NUMBER, rc);
        exit(-1);
    }

    // Create and start thread S2 with FIFO priority 9
    thread_args[1] = 2;
    param.sched_priority = 9;
    pthread_attr_setschedparam(&attr, &param);
    rc = pthread_create(&threads[1], &attr, thread_s2, (void *)&thread_args[1]);
    if (rc) {
        syslog(LOG_ERR, "[COURSE:%d][ASSIGNMENT:%d] ERROR; return code from pthread_create() is %d", COURSE_NUMBER, ASSIGNMENT_NUMBER, rc);
        exit(-1);
    }

    // Create and start thread S3 with FIFO priority 8
    thread_args[2] = 3;
    param.sched_priority = 8;
    pthread_attr_setschedparam(&attr, &param);
    rc = pthread_create(&threads[2], &attr, thread_s3, (void *)&thread_args[2]);
    if (rc) {
        syslog(LOG_ERR, "[COURSE:%d][ASSIGNMENT:%d] ERROR; return code from pthread_create() is %d", COURSE_NUMBER, ASSIGNMENT_NUMBER, rc);
        exit(-1);
    }

    // Create and start thread S4 with FIFO priority 7
    thread_args[3] = 4;
    param.sched_priority = 7;
    pthread_attr_setschedparam(&attr, &param);
    rc = pthread_create(&threads[3], &attr, thread_s4, (void *)&thread_args[3]);
    if (rc) {
        syslog(LOG_ERR, "[COURSE:%d][ASSIGNMENT:%d] ERROR; return code from pthread_create() is %d", COURSE_NUMBER, ASSIGNMENT_NUMBER, rc);
        exit(-1);
    }

    // Join the threads to ensure they complete execution
    for (int i = 0; i < 4; i++) {
        pthread_join(threads[i], NULL);
    }

    // Clean up and close syslog
    pthread_attr_destroy(&attr);
    closelog();

    return 0;
}
