#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/ip_icmp.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <pthread.h>

#define NUM_THREADS 4

typedef struct Task
{
    int a, b;
} Task;

Task taskQueue[256];
int taskCount = 0;
int terminate = 0;

pthread_mutex_t mutex_queue;
pthread_cond_t cond_queue;

void execute_task(Task *task)
{   
    usleep(50000);
    // replace this with some long running operation
    int result = task->a + task->b;
    printf("Result: %d + %d = %d\n", task->a, task->b, result);
}

void submit_task(Task task)
{
    pthread_mutex_lock(&mutex_queue);
    taskQueue[taskCount] = task;
    taskCount++;
    pthread_mutex_unlock(&mutex_queue);
    pthread_cond_signal(&cond_queue);
}

void *start_thread(void *args)
{
    while (1)
    {
        Task task;
        // critical section
        pthread_mutex_lock(&mutex_queue);

        while (taskCount == 0 && !terminate)
        {
            pthread_cond_wait(&cond_queue, &mutex_queue);
        }
        if(terminate && taskCount == 0){
            pthread_mutex_unlock(&mutex_queue);
            break;
        }

        task = taskQueue[0];
        int i;
        for (i = 0; i < taskCount - 1; i++)
        {
            taskQueue[i] = taskQueue[i + 1];
        }
        taskCount--;
        // end critical section
        pthread_mutex_unlock(&mutex_queue);
        execute_task(&task);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    Task t1 = {
        .a = 5,
        .b = 10};

    execute_task(&t1);
    // create the thread pool
    pthread_t th[NUM_THREADS];
    pthread_mutex_init(&mutex_queue, NULL);
    pthread_cond_init(&cond_queue, NULL);

    int i = 0;
    for (i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_create(&th[i], NULL, &start_thread, NULL) != 0)
        {
            perror("Failed to create thread ");
        }
    }

    srand(time(NULL));
    for (i = 0; i < 256; i++)
    {
        Task t = {
            .a = rand() % 100,
            .b = 10};
        submit_task(t);
    }
    pthread_mutex_lock(&mutex_queue);
    terminate = 1;
    pthread_mutex_unlock(&mutex_queue);
    pthread_cond_broadcast(&cond_queue);
    for (int i = 0; i < NUM_THREADS; i++)
    {
        if (pthread_join(th[i], NULL) != 0)
        {
            perror("Failed to join thread");
        }
    }
    pthread_mutex_destroy(&mutex_queue);
    pthread_cond_destroy(&cond_queue);
    printf("Complete\n");

    return 0;
}