#include <stdio.h>
#include "request.h"
#include "io_helper.h"
#include "circular_queue.h"
#include <semaphore.h>

#define THREAD_NUM 10

circular_queue_t *queue;
sem_t sem_full;
sem_t sem_empty;

char default_root[] = ".";

void thread_func();

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	if (queue_init(10, &queue) == -1)
	{
		perror("Failed to initialize circular queue");
		exit(1);
	}

	sem_init(&sem_full, 0, 0);
	sem_init(&sem_empty, 0, THREAD_NUM);

	pthread_t thread_id[THREAD_NUM];
	for (int i = 0; i < THREAD_NUM; i++)
	{
		if (pthread_create(&thread_id[i], NULL, (void *)thread_func, NULL) != 0)
		{
			perror("Failed to create thread");
			exit(1);
		}
	}
    
    while ((c = getopt(argc, argv, "d:p:")) != -1)
	switch (c) {
	case 'd':
	    root_dir = optarg;
	    break;
	case 'p':
	    port = atoi(optarg);
	    break;
	default:
	    fprintf(stderr, "usage: wserver [-d basedir] [-p port]\n");
	    exit(1);
	}

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);
    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		sem_wait(&sem_empty);
		if (push(conn_fd, queue) == -1)
		{
			perror("Failed to push to circular queue");
			exit(1);
		}
		sem_post(&sem_full);
    }
    return 0;
}

void
thread_func()
{
	int fd;
	while (1)
	{
		sem_wait(&sem_full);
		if (pop(&fd, queue) == -1)
		{
			perror("Failed to pop from circular queue");
			exit(1);
		}
		request_handle(fd);
		close_or_die(fd);
		sem_post(&sem_empty);
	}
}


    


 
