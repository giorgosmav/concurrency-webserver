#include "circular_queue.h"

int fd;
pthread_mutex_t m_lock;

int
queue_init(int size, circular_queue_t **queue)
{   
    if (size <= 0)
    {
        fprintf(stderr, "size must be greater than 0\n");
        return EXIT_FAILURE;
    }
    *queue = (circular_queue_t *)malloc(sizeof(circular_queue_t));
    if (*queue == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for circular queue\n");
        return EXIT_FAILURE;
    }
    (*queue)->array = (int *)malloc(size * sizeof(int));
    if ((*queue)->array == NULL)
    {
        fprintf(stderr, "Failed to allocate memory for circular queue array\n");
        free(*queue);
        return EXIT_FAILURE;
    }

    (*queue)->size = 0;
    (*queue)->max_size = size;
    (*queue)->head = -1;
    (*queue)->tail = -1;
    pthread_mutex_init(&m_lock, NULL);
    return EXIT_SUCCESS;
}

int
push(int fd, circular_queue_t *queue)
{
    if (queue == NULL)
    {
        fprintf(stderr, "queue is not initialized\n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    if (queue->size == queue->max_size)
    {
        pthread_mutex_unlock(&m_lock);
        return -1;
    }
    if (queue->head == -1)
    {
        queue->head = 0;
        queue->tail = 0;
    }
    else
    {
        queue->tail = (queue->tail + 1) % queue->max_size;
    }
    queue->array[queue->tail] = fd;
    queue->size++;
    pthread_mutex_unlock(&m_lock);
    return 0;
}

int
pop(int *fd, circular_queue_t *queue)
{
    if (queue == NULL)
    {
        fprintf(stderr, "queue is not initialized\n");
        return -1;
    }
    pthread_mutex_lock(&m_lock);
    if (queue->size == 0)
    {
        pthread_mutex_unlock(&m_lock);
        return -1;
    }
    *fd = queue->array[queue->head];
    if (queue->head == queue->tail)
    {
        queue->head = -1;
        queue->tail = -1;
    }
    else
    {
        queue->head = (queue->head + 1) % queue->max_size;
    }
    queue->size--;
    pthread_mutex_unlock(&m_lock);
    return 0;
}