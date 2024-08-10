#define _GNU_SOURCE  // Required for sched_getcpu function
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sched.h>
#include <syslog.h>
#include <string.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <time.h>

// Define constants representing the course and assignment numbers
#define COURSE_ID 2
#define TASK_ID 5

// Define the Least Common Multiple (LCM) period for the thread schedules
#define CYCLE_PERIOD 10

// Function to simulate some work by sleeping for a specified duration (in milliseconds)
void simulate_task(int duration) {
    usleep(duration * 1000); // Sleep for 'duration' milliseconds
}

// Function to log the start of a thread, including system information
void record_thread_start(int tid) {
    struct utsname system_info; // Structure to hold system information
    uname(&system_info);  // Get system information

    time_t now = time(NULL);  // Get the current time
    struct tm *local_time = localtime(&now);  // Convert time to local time structure
    char timestamp[100];  // Buffer to hold formatted time
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", local_time);  // Format time

    int cpu_core = sched_getcpu();  // Get the CPU core on which the thread is running

    // Log the system name, node name, course ID, task ID, thread ID, timestamp, and CPU core
    syslog(LOG_INFO, "%s %s [COURSE:%d][TASK:%d]: Thread %d start @ %s on core %d",
           system_info.sysname, system_info.nodename, COURSE_ID, TASK_ID, tid, timestamp, cpu_core);
}

// Thread function for the first service, which simulates a task every 2ms (50Hz)
void *thread_function1(void *tid) {
    int thread_id = *(int *)tid, period = 2, compute_time = 1;  // Set period and WCET for S1
    struct timespec next_run;  // Structure to hold the next activation time
    clock_gettime(CLOCK_MONOTONIC, &next_run);  // Get the current time in monotonic clock
    time_t end_time = time(NULL) + CYCLE_PERIOD;  // Calculate when the thread should stop running

    while (time(NULL) < end_time) {  // Loop until the end time is reached
        record_thread_start(thread_id);  // Log thread start
        simulate_task(compute_time);  // Simulate the workload
        next_run.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_run, NULL);  // Sleep until the next activation time
    }
    pthread_exit(NULL);  // Exit the thread when done
}

// Thread function for the second service, which simulates a task every 5ms (20Hz)
void *thread_function2(void *tid) {
    int thread_id = *(int *)tid, period = 5, compute_time = 2;  // Set period and WCET for S2
    struct timespec next_run;  // Structure to hold the next activation time
    clock_gettime(CLOCK_MONOTONIC, &next_run);  // Get the current time in monotonic clock
    time_t end_time = time(NULL) + CYCLE_PERIOD;  // Calculate when the thread should stop running

    while (time(NULL) < end_time) {  // Loop until the end time is reached
        record_thread_start(thread_id);  // Log thread start
        simulate_task(compute_time);  // Simulate the workload
        next_run.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_run, NULL);  // Sleep until the next activation time
    }
    pthread_exit(NULL);  // Exit the thread when done
}

// Thread function for the third service, which simulates a task every 10ms (10Hz)
void *thread_function3(void *tid) {
    int thread_id = *(int *)tid, period = 10, compute_time = 1;  // Set period and WCET for S3
    struct timespec next_run;  // Structure to hold the next activation time
    clock_gettime(CLOCK_MONOTONIC, &next_run);  // Get the current time in monotonic clock
    time_t end_time = time(NULL) + CYCLE_PERIOD;  // Calculate when the thread should stop running

    while (time(NULL) < end_time) {  // Loop until the end time is reached
        record_thread_start(thread_id);  // Log thread start
        simulate_task(compute_time);  // Simulate the workload
        next_run.tv_sec += period;  // Set the next activation time
        clock_nanosleep(CLOCK_MONOTONIC, TIMER_ABSTIME, &next_run, NULL);  // Sleep until the next activation time
    }
    pthread_exit(NULL);  // Exit the thread when done
}

// Function to log system information using the "uname" command
void log_system_info() {
    FILE *fp;
    char buffer[256];

    // Run the "uname -a" command and open a pipe to read the output
    if ((fp = popen("uname -a", "r")) == NULL) {
        syslog(LOG_ERR, "[COURSE:%d][TASK:%d] uname failed", COURSE_ID, TASK_ID);
        return;
    }

    // Read and log each line of the command output
    while (fgets(buffer, sizeof(buffer), fp) != NULL) {
        syslog(LOG_INFO, "[COURSE:%d][TASK:%d] %s", COURSE_ID, TASK_ID, buffer);
    }

    pclose(fp);  // Close the pipe
}

// Main function to create and manage threads
int main() {
    pthread_t threads[3];  // Array to hold thread identifiers for 3 services
    int thread_args[3], result;  // Array to hold thread arguments and result code
    pthread_attr_t attributes;  // Attribute object for setting thread properties
    struct sched_param parameters;  // Scheduler parameters

    openlog("task_syslog", LOG_PID | LOG_CONS, LOG_USER);  // Initialize syslog

    log_system_info();  // Log system information

    pthread_attr_init(&attributes);  // Initialize thread attributes
    pthread_attr_setschedpolicy(&attributes, SCHED_FIFO);  // Set scheduling policy to FIFO

    // Create and start the first thread
    thread_args[0] = 1;
    parameters.sched_priority = 10;  // Set thread priority
    pthread_attr_setschedparam(&attributes, &parameters);  // Apply priority to attributes
    if ((result = pthread_create(&threads[0], &attributes, thread_function1, (void *)&thread_args[0])) != 0) {
        syslog(LOG_ERR, "[COURSE:%d][TASK:%d] pthread_create() failed: %d", COURSE_ID, TASK_ID, result);
        exit(-1);  // Exit if thread creation fails
    }

    // Create and start the second thread
    thread_args[1] = 2;
    parameters.sched_priority = 9;  // Set thread priority
    pthread_attr_setschedparam(&attributes, &parameters);  // Apply priority to attributes
    if ((result = pthread_create(&threads[1], &attributes, thread_function2, (void *)&thread_args[1])) != 0) {
        syslog(LOG_ERR, "[COURSE:%d][TASK:%d] pthread_create() failed: %d", COURSE_ID, TASK_ID, result);
        exit(-1);  // Exit if thread creation fails
    }

    // Create and start the third thread
    thread_args[2] = 3;
    parameters.sched_priority = 8;  // Set thread priority
    pthread_attr_setschedparam(&attributes, &parameters);  // Apply priority to attributes
    if ((result = pthread_create(&threads[2], &attributes, thread_function3, (void *)&thread_args[2])) != 0) {
        syslog(LOG_ERR, "[COURSE:%d][TASK:%d] pthread_create() failed: %d", COURSE_ID, TASK_ID, result);
        exit(-1);  // Exit if thread creation fails
    }

    // Join all threads to ensure they complete execution
    for (int i = 0; i < 3; i++) {
        pthread_join(threads[i], NULL);
    }

    pthread_attr_destroy(&attributes);  // Destroy the attribute object
    closelog();  // Close the syslog

    return 0;
}
