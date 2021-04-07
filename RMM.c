#include <pthread.h>
#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

static volatile int flag1 = 0; // volatile prevents compiler optimizations
static volatile int flag2 = 0;
static volatile int turn = 1;
static volatile int SharedCounter = 0;
static volatile int Iterations=0;

void *task1(void *arg) {
printf("Starting task 1\n");
for(int i=0; i<Iterations; i++) {
flag1 = 1;
turn = 2;
__sync_synchronize();
while((flag2 == 1) && (turn == 2)) //skip
;
SharedCounter++;
flag1 = 0;
}
printf("Task1 terminated\n");
return 0;
}

void *task2(void *arg) {
printf("Starting task 2\n");
for(int i=0; i<Iterations; i++) {
flag2 = 1;
turn = 1;
__sync_synchronize();
while((flag1 == 1) && (turn == 1)) //skip
;
SharedCounter++;
flag2 = 0;
}
printf("Task2 terminated\n");
return 0;
}

int main(int argc, char ** argv)
{
pthread_t my_thread_1;
pthread_t my_thread_2;
void * returnCode;
int result;
int expected_sum;

if(argc != 2) {
fprintf(stderr, "USAGE: %s <loopcount>\n", argv[0]);
exit(1);
}

Iterations = atoi(argv[1]);
expected_sum = 2*Iterations;

/* Start the threads */
result = pthread_create(&my_thread_1, NULL, task1, NULL);
result = pthread_create(&my_thread_2, NULL, task2, NULL);

/* Wait for the threads to end */
result = pthread_join(my_thread_1,&returnCode);
result = pthread_join(my_thread_2,&returnCode);
printf("Both threads terminated\n");

/* Check result */
if( SharedCounter != expected_sum ) {
printf("[Anomaly] Sum %d != Expected value %d.\n", SharedCounter, expected_sum);
return 1;
} else {
printf("[Ok] Sum %d = Expected value %d.\n", SharedCounter, expected_sum);
return 0;
}
}