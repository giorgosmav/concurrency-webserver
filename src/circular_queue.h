#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    int head;
    int tail;
    int size;
    int max_size;
    int *array;
}circular_queue_t;

extern int fd;
extern pthread_mutex_t m_lock;

int queue_init(int, circular_queue_t **);
    
int push(int fd, circular_queue_t *);

int pop(int *fd, circular_queue_t *);

