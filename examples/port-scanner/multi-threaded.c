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

#define NUM_THREADS 16
#define INIT_QUEUE_CAPACITY 256

typedef struct
{
    const char *ip;
    int start_port;
    int end_port;
} ScannerArgs;

ScannerArgs scanner_args;

void validate_port(int port)
{
    if (port < 1 || port > 65535)
    {
        printf("Invalid port number: %d. Port number must be between 1 and 65535.\n", port);
        exit(1);
    }
}

void validate_args(int argc, char *argv[], ScannerArgs *scanner_args)
{
    if (argc < 4)
    {
        printf("Usage: %s <IP> <start_port> <end_port>\n", argv[0]);
        exit(1);
    }

    scanner_args->ip = argv[1];
    scanner_args->start_port = atoi(argv[2]);
    scanner_args->end_port = atoi(argv[3]);

    validate_port(scanner_args->start_port);
    validate_port(scanner_args->end_port);

    if (scanner_args->end_port < scanner_args->start_port)
    {
        printf("Invalid port numbers. Starting port must be less than end port\n");
        exit(1);
    }
}

int scan_port(int port)
{
    struct sockaddr_in server_address;
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(port);
    if (inet_pton(AF_INET, scanner_args.ip, &server_address.sin_addr) <= 0)
    {
        printf("Invalid address/ Address not supported\n");
        return 1;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        printf("Socket creation error\n");
        return 1;
    }

    int connection = connect(sockfd, (struct sockaddr *)&server_address, sizeof(server_address));
    close(sockfd);
    return connection;
}
typedef struct Task
{
    int port;
} Task;

typedef struct TaskQueue
{
    Task *tasks;
    int size;
    int capacity;

} TaskQueue;
TaskQueue taskQueue;

int terminate = 0;

pthread_mutex_t mutex_queue;
pthread_cond_t cond_queue;

void init_task_queue(TaskQueue *queue, int capacity)
{
    queue->tasks = (Task *)malloc(capacity * sizeof(Task));
    queue->size = 0;
    queue->capacity = capacity;
}

void resize_task_queue(TaskQueue *queue)
{
    queue->capacity *= 2;
    queue->tasks = (Task *)realloc(queue->tasks, queue->capacity * sizeof(Task));
}

void free_task_queue(TaskQueue *queue)
{
    free(queue->tasks);
}

void execute_task(Task *task)
{
    int connection = scan_port(task->port);
    if (connection == 1)
    {
        printf("warn: error creating socket\n");
        exit(1);
    }

    if (connection == 0)
    {
        printf("%s:%d/TCP: connection success\n", scanner_args.ip, task->port);
    }
}

void submit_task(Task task)
{
    pthread_mutex_lock(&mutex_queue);

    if (taskQueue.size == taskQueue.capacity)
    {
        resize_task_queue(&taskQueue);
    }
    taskQueue.tasks[taskQueue.size++] = task;
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

        while (taskQueue.size == 0 && !terminate)
        {
            pthread_cond_wait(&cond_queue, &mutex_queue);
        }
        if (terminate && taskQueue.size == 0)
        {
            // all jobs have been completed / claimed
            pthread_mutex_unlock(&mutex_queue);
            break;
        }

        task = taskQueue.tasks[0];
        memmove(&taskQueue.tasks[0], &taskQueue.tasks[1], (taskQueue.size - 1) * sizeof(Task));
        taskQueue.size--;
        // end critical section
        pthread_mutex_unlock(&mutex_queue);
        execute_task(&task);
    }
    return NULL;
}

int main(int argc, char *argv[])
{
    validate_args(argc, argv, &scanner_args);
    init_task_queue(&taskQueue, INIT_QUEUE_CAPACITY);
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

    for (int i = scanner_args.start_port; i <= scanner_args.end_port; i++)
    {
        Task t = {
            .port = i};
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
    free_task_queue(&taskQueue);
    printf("Complete\n");
    return 0;
}